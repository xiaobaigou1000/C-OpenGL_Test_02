#include "MyGLWindow.h"

#include<QKeyEvent>
#include<cmath>

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

MyGLWindow::MyGLWindow(QWidget* parent)
    : QOpenGLWidget(parent)
{
    resize(1920, 1080);
    setWindowFlags(Qt::FramelessWindowHint);
    setCursor(Qt::BlankCursor);
    setMouseTracking(true);
}

MyGLWindow::~MyGLWindow()
{
}

void MyGLWindow::initializeGL()
{
    programBeginPoint = lastTimePoint = std::chrono::steady_clock::now();
    boxCamera.caculateCamera();
    std::default_random_engine dre( std::chrono::system_clock::now().time_since_epoch().count() );

    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    background.init();

    lightBoxShader.create();
    lightBoxShader.addShaderFromSourceFile(QOpenGLShader::Vertex, "boxShader.vert");
    lightBoxShader.addShaderFromSourceFile(QOpenGLShader::Fragment, "lightBox.frag");
    lightBoxShader.link();
    lightBox.init();
    lightBox.resetScaleMat(scale(mat4(1.0f), vec3{ 0.2f }));
    std::normal_distribution<float> lightPosDistribution{ 3.0f, 3.0f };
    for (int i = 0; i < 4; ++i)
    {
        pointLightColor.push_back({ 1.0f,1.0f,1.0f });
        lightPos.push_back({ lightPosDistribution(dre),lightPosDistribution(dre),lightPosDistribution(dre) });
    }

    boxShader.create();
    boxShader.addShaderFromSourceFile(QOpenGLShader::Vertex, "boxShader.vert");
    boxShader.addShaderFromSourceFile(QOpenGLShader::Fragment, "boxShader.frag");
    boxShader.link();
    myBox.init();
    myBox.resetRotateMat(rotate(mat4{ 1.0f }, radians(55.0f), vec3{ 1.0f,1.0f,0.0f }));

    boxTexture = new QOpenGLTexture(QImage("./images/container2.png").mirrored());
    boxSpecular = new QOpenGLTexture(QImage("./images/container2_specular.png").mirrored());
    emissionMap = new QOpenGLTexture(QImage("./images/matrix.jpg").mirrored());

    std::normal_distribution<float> positionDistribution(3.0f, 3.0f);
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
    glDisable(GL_DEPTH_TEST);
    background.draw();
    glEnable(GL_DEPTH_TEST);

    boxCamera.caculateCamera();
    auto currentTime = std::chrono::steady_clock::now();
    auto passedDuration = duration_cast<duration<float, std::ratio<1>>>(currentTime - lastTimePoint);
    auto timeFromBeginPoint = duration_cast<duration<float, std::ratio<1>>>(currentTime - programBeginPoint);
    lastTimePoint = currentTime;

    vec3 lightColor{ 1.0f, 1.0f, 1.0f };
    lightColor.r = 0.5f + 0.5f * sin(timeFromBeginPoint.count());
    lightColor.g = 0.5f + 0.5f * cos(timeFromBeginPoint.count());
    lightColor.b = 1.0f - lightColor.r;
    vec3 diffuseColor = lightColor * vec3{ 0.7f };
    vec3 ambientColor = diffuseColor * vec3{ 0.3f };

    lightBoxShader.bind();
    lightBox.bind();
    glUniform3fv(lightBoxShader.uniformLocation("lightColor"), 1, value_ptr(lightColor));
    for (int i = 0; i <lightPos.size(); ++i)
    {
        pointLightColor[i].r = 0.5f + 0.5f * sin(0.5f+timeFromBeginPoint.count()+ 0.5f*i);
        pointLightColor[i].g = 0.5f + 0.5f * cos(0.5f + timeFromBeginPoint.count() + 0.5f * i);
        pointLightColor[i].b = 1.0f - pointLightColor[i].r;
        glUniform3fv(lightBoxShader.uniformLocation("lightColor"), 1, value_ptr(pointLightColor[i]));

        mat4 lightModel = translate(mat4{ 1.0f }, lightPos[i]) * lightBox.scaleMat;
        glUniformMatrix4fv(lightBoxShader.uniformLocation("MVP"), 1, GL_FALSE, value_ptr(boxCamera.viewProjectionMat() * lightModel));
        lightBox.draw();
    }

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
    glActiveTexture(GL_TEXTURE2);
    emissionMap->bind(GL_TEXTURE_2D);
    glUniform1i(boxShader.uniformLocation("material.emission"), 2);
    glUniform1f(boxShader.uniformLocation("material.shininess"), 32.0f);
    setLightVariableForBoxShader(ambientColor, diffuseColor);

    for (int i = 0; i < 10; ++i)
    {
        mat4 modelMat = translateMatrices[i] * rotateMatrices[i];
        rotateMatrices[i] = rotate(rotateMatrices[i], radians(passedDuration.count() * 30.0f), rotateAxis[i]);
        glUniformMatrix4fv(boxShader.uniformLocation("MVP"), 1, GL_FALSE, value_ptr(boxCamera.viewProjectionMat() * modelMat));
        glUniformMatrix4fv(boxShader.uniformLocation("modelMat"), 1, GL_FALSE, value_ptr(modelMat));
        myBox.draw();
    }

    update();
}

void MyGLWindow::resizeGL(int w, int h)
{
    background.resize(w, h);
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

void MyGLWindow::setLightVariableForBoxShader(vec3 ambientColor, vec3 diffuseColor)
{
    vec3 spotLightColor{ 1.0f, 1.0f, 1.0f };
    vec3 spotDiffuseColor = spotLightColor * vec3{ 0.7f };
    vec3 spotAmbientColor = spotDiffuseColor * vec3{ 0.3f };

    glUniform3f(boxShader.uniformLocation("dirlight.direction"), -0.2f, -1.0f, -0.3f);
    glUniform3fv(boxShader.uniformLocation("dirlight.ambient"), 1, value_ptr(ambientColor));
    glUniform3fv(boxShader.uniformLocation("dirlight.diffuse"), 1, value_ptr(diffuseColor));
    glUniform3f(boxShader.uniformLocation("dirlight.specular"), 1.0f, 1.0f, 1.0f);

    for (int i = 0; i < 4; ++i)
    {
        vec3 pointLightDiffuseColor = pointLightColor[i] * vec3{ 0.7f };
        vec3 pointLightAmbientColor = pointLightDiffuseColor * vec3{ 0.1f };

        QString lightName = QString::fromStdString("pointlights[" + std::to_string(i) + "].");
        glUniform3fv(boxShader.uniformLocation(lightName + "position"), 1, value_ptr(lightPos[i]));
        glUniform1f(boxShader.uniformLocation(lightName + "constant"), 1.0f);
        glUniform1f(boxShader.uniformLocation(lightName + "linear"), 0.09f);
        glUniform1f(boxShader.uniformLocation(lightName + "quadratic"), 0.032f);
        glUniform3fv(boxShader.uniformLocation(lightName + "ambient"), 1, value_ptr(pointLightAmbientColor));
        glUniform3fv(boxShader.uniformLocation(lightName + "diffuse"), 1, value_ptr(pointLightDiffuseColor));
        glUniform3f(boxShader.uniformLocation(lightName + "specular"), 1.0f, 1.0f, 1.0f);
    }

    glUniform3fv(boxShader.uniformLocation("spotlight.position"), 1, value_ptr(boxCamera.position));
    glUniform3fv(boxShader.uniformLocation("spotlight.direction"), 1, value_ptr(boxCamera.front));
    glUniform1f(boxShader.uniformLocation("spotlight.cutOff"), cosf(radians(12.5f)));
    glUniform1f(boxShader.uniformLocation("spotlight.outerCutOff"), cosf(radians(17.5f)));
    glUniform3fv(boxShader.uniformLocation("spotlight.ambient"), 1, value_ptr(spotAmbientColor));
    glUniform3fv(boxShader.uniformLocation("spotlight.diffuse"), 1, value_ptr(spotDiffuseColor));
    glUniform3f(boxShader.uniformLocation("spotlight.specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(boxShader.uniformLocation("spotlight.constant"), 1.0f);
    glUniform1f(boxShader.uniformLocation("spotlight.linear"), 0.09f);
    glUniform1f(boxShader.uniformLocation("spotlight.quadratic"), 0.032f);
}
