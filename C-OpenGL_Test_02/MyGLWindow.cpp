#include "MyGLWindow.h"

#include<qimage.h>
#include<QKeyEvent>
#include<cmath>
#include<memory>

using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using glm::mat4;
using glm::mat3;
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
    std::default_random_engine dre(std::chrono::system_clock::now().time_since_epoch().count());

    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    //code here
    glPrimitiveRestartIndex(0xFFFF);

    glGenBuffers(1, &adt.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, adt.vbo);
    glBufferStorage(GL_ARRAY_BUFFER, adt.vertices.size() * sizeof(float), nullptr, GL_MAP_WRITE_BIT);
    float* ptr = static_cast<float*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
    memcpy(ptr, adt.vertices.data(), adt.vertices.size() * sizeof(float));
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glGenVertexArrays(1, &adt.vao);
    glBindVertexArray(adt.vao);
    glBindBuffer(GL_ARRAY_BUFFER, adt.vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    adt.shader.create();
    adt.shader.addShaderFromSourceFile(QOpenGLShader::Vertex, "./shaders/pointsGeometry.vert");
    adt.shader.addShaderFromSourceFile(QOpenGLShader::Geometry, "./shaders/pointsGeometry.geom");
    adt.shader.addShaderFromSourceFile(QOpenGLShader::Fragment, "./shaders/pointsGeometry.frag");
    adt.shader.link();
}

void MyGLWindow::paintGL()
{
    mainCamera.caculateCamera();
    auto currentTime = std::chrono::steady_clock::now();
    auto passedDuration = duration_cast<duration<float>>(currentTime - lastTimePoint).count();
    auto timeFromBeginPoint = duration_cast<duration<float>>(currentTime - programBeginPoint).count();
    lastTimePoint = currentTime;

    //code here
    adt.shader.bind();
    glUniform4f(adt.shader.uniformLocation("color"), 0.0f, 1.0f, 0.0f, 1.0f);
    glBindVertexArray(adt.vao);
    glDrawArrays(GL_POINTS, 0, 4);

    update();
}

void MyGLWindow::resizeGL(int w, int h)
{
    mainCamera.resizeCamera(w, h);
}

void MyGLWindow::mouseMoveEvent(QMouseEvent* event)
{
    float xAxisMove = event->x() - width() / 2;
    float yAxisMove = event->y() - height() / 2;
    QCursor myCursor = cursor();
    myCursor.setPos(mapToGlobal({ width() / 2, height() / 2 }));
    setCursor(myCursor);
    if (std::abs(xAxisMove) > 150.0f || std::abs(yAxisMove) > 150.0f)
    {
        return;
    }
    mainCamera.processMouseMovement(xAxisMove, yAxisMove);
}

void MyGLWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        close();
    if (event->key() == Qt::Key_W)
        mainCamera.setKeyW(true);
    if (event->key() == Qt::Key_S)
        mainCamera.setKeyS(true);
    if (event->key() == Qt::Key_A)
        mainCamera.setKeyA(true);
    if (event->key() == Qt::Key_D)
        mainCamera.setKeyD(true);
}

void MyGLWindow::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_W)
        mainCamera.setKeyW(false);
    if (event->key() == Qt::Key_S)
        mainCamera.setKeyS(false);
    if (event->key() == Qt::Key_A)
        mainCamera.setKeyA(false);
    if (event->key() == Qt::Key_D)
        mainCamera.setKeyD(false);
}
