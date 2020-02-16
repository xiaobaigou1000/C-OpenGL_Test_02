#include "MyGLWindow.h"

#include<QKeyEvent>
#include<cmath>

MyGLWindow::MyGLWindow(QWidget* parent)
    : QOpenGLWidget(parent)
{
    resize(800, 800);
    setCursor(Qt::BlankCursor);
    setMouseTracking(true);
}

MyGLWindow::~MyGLWindow()
{
}

void MyGLWindow::initializeGL()
{
    using glm::mat4;
    using glm::vec3;
    using glm::translate;
    using glm::scale;
    using glm::value_ptr;
    using glm::rotate;
    using glm::radians;

    programBeginPoint = lastTimePoint = std::chrono::steady_clock::now();
    boxCamera.caculateCamera();

    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    lightBoxShader.create();
    lightBoxShader.addShaderFromSourceFile(QOpenGLShader::Vertex, "boxShader.vert");
    lightBoxShader.addShaderFromSourceFile(QOpenGLShader::Fragment, "lightBox.frag");
    lightBoxShader.link();
    lightBox.init();
    lightBox.resetTranslateMat(translate(mat4(1.0f), lightPos));
    lightBox.resetScaleMat(scale(mat4(1.0f), vec3{ 0.2f }));

    boxShader.create();
    boxShader.addShaderFromSourceFile(QOpenGLShader::Vertex, "boxShader.vert");
    boxShader.addShaderFromSourceFile(QOpenGLShader::Fragment, "boxShader.frag");
    boxShader.link();
    myBox.init();
    myBox.resetRotateMat(rotate(mat4{1.0f}, radians(55.0f), vec3{ 1.0f,1.0f,0.0f }));

    boxTexture = new QOpenGLTexture(QImage("./images/container2.png").mirrored());
    boxSpecular = new QOpenGLTexture(QImage("./images/container2_specular.png").mirrored());

    std::default_random_engine dre;
    std::uniform_real_distribution<float> positionDistribution(0.0f, 6.0f);
    std::uniform_real_distribution<float> axisDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> angleDist(0.0f, 60.0f);

    for (int i = 0; i < 10; ++i)
    {
        vec3 boxPos{ positionDistribution(dre),positionDistribution(dre),positionDistribution(dre) };
        vec3 boxRotateAxis{ axisDist(dre),axisDist(dre),axisDist(dre) };
        translateMatrices.push_back(translate(mat4{ 1.0f }, boxPos));
        rotateAxis.push_back(boxRotateAxis);
        rotateMatrices.push_back(rotate(mat4{ 1.0f }, angleDist(dre), boxRotateAxis));
    }
}

void MyGLWindow::paintGL()
{
    using std::chrono::steady_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using glm::mat4;
    using glm::vec3;
    using glm::translate;
    using glm::scale;
    using glm::value_ptr;
    using glm::rotate;
    using glm::radians;
    using glm::normalize;
    
    boxCamera.caculateCamera();
    auto currentTime = std::chrono::steady_clock::now();
    auto passedDuration = duration_cast<duration<float, std::ratio<1>>>(currentTime - lastTimePoint);
    auto timeFromBeginPoint = duration_cast<duration<float, std::ratio<1>>>(currentTime - programBeginPoint);

    vec3 lightColor{ 1.0f, 1.0f, 1.0f };
    vec3 diffuseColor = lightColor * vec3{ 0.7f };
    vec3 ambientColor = diffuseColor * vec3{ 0.3f };

    lightBoxShader.bind();
    lightBox.bind();
    //lightPos.y = 0.0f;
    //lightPos.x = 2.0f * sinf(timeFromBeginPoint.count());
    //lightPos.z = 2.0f * cosf(timeFromBeginPoint.count());
    lightBox.resetTranslateMat(translate(mat4{ 1.0f }, lightPos));
    glUniform3fv(lightBoxShader.uniformLocation("lightColor"), 1, value_ptr(lightColor));
    glUniformMatrix4fv(lightBoxShader.uniformLocation("MVP"), 1, GL_FALSE, glm::value_ptr(boxCamera.viewProjectionMat()*lightBox.getModelMat()));
    lightBox.draw();

    myBox.bind();
    boxShader.bind();
    myBox.rotateMat = rotate(myBox.rotateMat, radians(20.0f * passedDuration.count()), vec3{ 1.0f,1.0f,0.0f });
    glUniform3fv(boxShader.uniformLocation("viewPos"), 1, value_ptr(boxCamera.position));
    glActiveTexture(GL_TEXTURE0);
    boxTexture->bind(GL_TEXTURE_2D);
    glUniform1i(boxShader.uniformLocation("material.diffuse"), 0);
    glActiveTexture(GL_TEXTURE1);
    boxSpecular->bind(GL_TEXTURE_2D);
    glUniform1i(boxShader.uniformLocation("material.specular"), 1);
    glUniform1f(boxShader.uniformLocation("material.shininess"), 32.0f);
    glUniform3fv(boxShader.uniformLocation("light.position"), 1, value_ptr(lightPos));
    glUniform3fv(boxShader.uniformLocation("light.ambient"),1,value_ptr(ambientColor));
    glUniform3fv(boxShader.uniformLocation("light.diffuse"),1,value_ptr(diffuseColor));
    glUniform3f(boxShader.uniformLocation("light.specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(boxShader.uniformLocation("light.constant"), 1.0f);
    glUniform1f(boxShader.uniformLocation("light.linear"), 0.09f);
    glUniform1f(boxShader.uniformLocation("light.quadratic"), 0.032f);

    for (int i = 0; i < 10; ++i)
    {
        mat4 modelMat = translateMatrices[i] * rotateMatrices[i];
        rotateMatrices[i] = rotate(rotateMatrices[i], radians(passedDuration.count() * 30.0f), rotateAxis[i]);
        glUniformMatrix4fv(boxShader.uniformLocation("MVP"), 1, GL_FALSE, glm::value_ptr(boxCamera.viewProjectionMat() * modelMat));
        glUniformMatrix4fv(boxShader.uniformLocation("modelMat"), 1, GL_FALSE, glm::value_ptr(modelMat));
        myBox.draw();
    }

    lastTimePoint = currentTime;
    update();
}

void MyGLWindow::resizeGL(int w, int h)
{
    boxCamera.resizeCamera(w, h);
}

void MyGLWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (!rect().contains(event->pos()))
        return;
    if (firstMouse)
    {
        QCursor myCursor = cursor();
        myCursor.setPos(mapToGlobal({ width() / 2, height() / 2 }));
        setCursor(myCursor);
        firstMouse = false;
        return;
    }
    if (secondMouse)
    {
        secondMouse = false;
        return;
    }
    float xAxisMove = event->x() - width() / 2;
    float yAxisMove = event->y() - height() / 2;
    boxCamera.processMouseMovement(xAxisMove, yAxisMove);
    QCursor myCursor = cursor();
    myCursor.setPos(mapToGlobal({ width() / 2, height() / 2 }));
    setCursor(myCursor);
}

void MyGLWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        close();
    if (event->key() == Qt::Key_W)
        boxCamera.setKeyW(true);
    if (event->key() == Qt::Key_S)
        boxCamera.setKeyS(true);
    if (event->key() == Qt::Key_A)
        boxCamera.setKeyA(true);
    if (event->key() == Qt::Key_D)
        boxCamera.setKeyD(true);
}

void MyGLWindow::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_W)
        boxCamera.setKeyW(false);
    if (event->key() == Qt::Key_S)
        boxCamera.setKeyS(false);
    if (event->key() == Qt::Key_A)
        boxCamera.setKeyA(false);
    if (event->key() == Qt::Key_D)
        boxCamera.setKeyD(false);
}
