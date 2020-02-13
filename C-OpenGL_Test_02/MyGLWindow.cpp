#include "MyGLWindow.h"

#include<fstream>

MyGLWindow::MyGLWindow(QWidget* parent)
    : QOpenGLWidget(parent)
{

}

MyGLWindow::~MyGLWindow()
{
}

void MyGLWindow::initializeGL()
{
    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    myTriangle.init();
    int maxVertexAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
    qInfo() << "GL_MAX_VERTEX_ATTRIBS : " << maxVertexAttribs;
}

void MyGLWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    myTriangle.draw();
    update();
}

void MyGLWindow::resizeGL(int w, int h)
{
}
