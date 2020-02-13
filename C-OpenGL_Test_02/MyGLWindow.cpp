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
    initTriangle();
}

void MyGLWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawTriangle();
    update();
}

void MyGLWindow::resizeGL(int w, int h)
{
}

void MyGLWindow::initTriangle()
{
    glCreateBuffers(1, &triangleVBO);
    glNamedBufferData(triangleVBO, triangleVertices.size()*sizeof(float) , triangleVertices.data(), GL_STATIC_DRAW);
    glCreateVertexArrays(1, &triangleVAO);
    glBindVertexArray(triangleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    triangleShader.create();
    triangleShader.addShaderFromSourceFile(QOpenGLShader::Vertex, "triangleVertexShader.vert");
    triangleShader.addShaderFromSourceFile(QOpenGLShader::Fragment, "triangleFragmentShader.frag");
    triangleShader.link();
    triangleShader.bind();
}

void MyGLWindow::drawTriangle()
{
    glBindVertexArray(triangleVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
