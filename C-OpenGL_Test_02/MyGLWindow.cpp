#include "MyGLWindow.h"

#include<QKeyEvent>

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
    
    boxCamera.caculateCamera();

    lightBoxShader.bind();
    lightBox.bind();
    glUniformMatrix4fv(lightBoxShader.uniformLocation("MVP"), 1, GL_FALSE, glm::value_ptr(boxCamera.viewProjectionMat()*lightBox.getModelMat()));
    lightBox.draw();

    myBox.bind();
    boxShader.bind();
    auto currentTime = std::chrono::steady_clock::now();
    auto passedDuration = duration_cast<duration<float, std::ratio<1>>>(currentTime - lastTimePoint);
    myBox.rotateMat = rotate(myBox.rotateMat, radians(20.0f * passedDuration.count()), vec3{ 1.0f,1.0f,0.0f });
    lastTimePoint = currentTime;

    glUniformMatrix4fv(boxShader.uniformLocation("MVP"), 1, GL_FALSE, glm::value_ptr(boxCamera.viewProjectionMat() * myBox.getModelMat()));
    glUniformMatrix4fv(boxShader.uniformLocation("modelMat"), 1, GL_FALSE, glm::value_ptr(myBox.getModelMat()));

    glUniform3f(boxShader.uniformLocation("objectColor"), 1.0f, 0.5f, 0.31f);
    glUniform3f(boxShader.uniformLocation("lightColor"), 1.0f, 1.0f, 1.0f);
    glUniform3f(boxShader.uniformLocation("lightPos"), lightPos.x, lightPos.y, lightPos.z);
    glUniform3fv(boxShader.uniformLocation("viewPos"), 1, value_ptr(boxCamera.position));
    myBox.draw();

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
