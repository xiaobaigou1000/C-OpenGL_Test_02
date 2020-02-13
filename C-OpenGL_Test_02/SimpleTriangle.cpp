#include "SimpleTriangle.h"

SimpleTriangle::SimpleTriangle()
{
}

void SimpleTriangle::initTriangle()
{
    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();
    glCreateBuffers(1, &triangleVBO);
    glNamedBufferData(triangleVBO, triangleVertices.size() * sizeof(float), triangleVertices.data(), GL_STATIC_DRAW);
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

void SimpleTriangle::drawTriangle()
{
    glBindVertexArray(triangleVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

