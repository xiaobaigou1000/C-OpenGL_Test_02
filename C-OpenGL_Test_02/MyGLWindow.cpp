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
using glm::perspective;
using glm::lookAt;

MyGLWindow::MyGLWindow(QWidget* parent)
    : QOpenGLWidget(parent)
{
    resize(800, 800);
    //setWindowFlags(Qt::FramelessWindowHint);
    setCursor(Qt::BlankCursor);
    setMouseTracking(true);
}

MyGLWindow::~MyGLWindow()
{
}

void MyGLWindow::initializeGL()
{
    programBeginPoint = lastTimePoint = std::chrono::steady_clock::now();
    std::default_random_engine dre( std::chrono::system_clock::now().time_since_epoch().count() );

    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    testModel.loadModel("./models/nanosuit/nanosuit.obj");
    testModel.init();
    modelShader.create();
    modelShader.addShaderFromSourceFile(QOpenGLShader::Vertex, "model.vert");
    modelShader.addShaderFromSourceFile(QOpenGLShader::Fragment, "model.frag");
    modelShader.link();
}

void MyGLWindow::paintGL()
{

    boxCamera.caculateCamera();
    auto currentTime = std::chrono::steady_clock::now();
    auto passedDuration = duration_cast<duration<float, std::ratio<1>>>(currentTime - lastTimePoint);
    auto timeFromBeginPoint = duration_cast<duration<float, std::ratio<1>>>(currentTime - programBeginPoint);
    lastTimePoint = currentTime;

    modelShader.bind();

    vec3 position;
    position.x = 5.0f * sin(timeFromBeginPoint.count());
    position.z = 5.0f * cos(timeFromBeginPoint.count());
    mat4 translateMat = translate(mat4{ 1.0f }, vec3(0.0f, -1.75f, 0.0f));
    mat4 scaleMat = scale(mat4{ 1.0f }, vec3(0.2f, 0.2f, 0.2f));
    mat4 modelMat = translateMat * scaleMat;
    

    glUniformMatrix4fv(modelShader.uniformLocation("MVP"), 1, GL_FALSE, value_ptr(boxCamera.viewProjectionMat()*modelMat));
    //glUniformMatrix4fv(modelShader.uniformLocation("modelMat"), 1, GL_FALSE, value_ptr(modelMat));
    testModel.draw(&modelShader);


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
