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
using glm::vec2;
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
    glGenBuffers(1, &instances.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, instances.vbo);
    glBufferStorage(GL_ARRAY_BUFFER, instances.vertices.size() * sizeof(float), instances.vertices.data(), 0);
    glGenVertexArrays(1, &instances.vao);
    glBindVertexArray(instances.vao);
    glBindBuffer(GL_ARRAY_BUFFER, instances.vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    instances.shader.create();
    instances.shader.addShaderFromSourceFile(QOpenGLShader::Vertex, "./shaders/instanceDrawTriangle.vert");
    instances.shader.addShaderFromSourceFile(QOpenGLShader::Fragment, "./shaders/instanceDrawTriangle.frag");
    instances.shader.link();

    std::array<vec2, 100> offsets;
    int index = 0;
    float off = 0.1f;
    for (int i = -10; i < 10; i+=2)
    {
        for (int j = -10; j < 10; j+=2)
        {
            offsets[index++] = vec2(static_cast<float>(j) / 10.0f + off, static_cast<float>(i) / 10.0f + off);
        }
    }
    glGenBuffers(1, &instances.slowVbo);
    glBindBuffer(GL_ARRAY_BUFFER, instances.slowVbo);
    glBufferStorage(GL_ARRAY_BUFFER, offsets.size() * sizeof(vec2), offsets.data(), 0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);
    instances.shader.bind();
    glUniform2fv(instances.shader.uniformLocation("offsets"), 100, (float*)(offsets.data()));
}

void MyGLWindow::paintGL()
{
    mainCamera.caculateCamera();
    auto currentTime = std::chrono::steady_clock::now();
    auto passedDuration = duration_cast<duration<float>>(currentTime - lastTimePoint).count();
    auto timeFromBeginPoint = duration_cast<duration<float>>(currentTime - programBeginPoint).count();
    lastTimePoint = currentTime;

    //code here
    instances.shader.bind();
    glBindVertexArray(instances.vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);

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
    if (std::abs(xAxisMove) < 150.0f && std::abs(yAxisMove) < 150.0f)
    {
        mainCamera.processMouseMovement(xAxisMove, yAxisMove);
    }
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
