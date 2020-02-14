#include "MyGLWindow.h"

MyGLWindow::MyGLWindow(QWidget* parent)
    : QOpenGLWidget(parent)
{
    resize(800, 800);
}

MyGLWindow::~MyGLWindow()
{
}

void MyGLWindow::initializeGL()
{
    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    myTriangle.init();
    myBox.init();
    int maxVertexAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
    qInfo() << "GL_MAX_VERTEX_ATTRIBS : " << maxVertexAttribs;
}

void MyGLWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    myTriangle.draw();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    myBox.draw();
    update();
}

void MyGLWindow::resizeGL(int w, int h)
{
    myTriangle.resize(w, h);
    myBox.resize(w, h);
}
