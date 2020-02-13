#include "SimpleTriangle.h"

SimpleTriangle::SimpleTriangle()
{
}

void SimpleTriangle::initTriangle()
{
    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();

    glCreateBuffers(1, &VBO);
    glNamedBufferData(VBO, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glCreateBuffers(1, &EBO);
    glNamedBufferData(EBO, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);
    
    glCreateVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    triangleShader.create();
    triangleShader.addShaderFromSourceFile(QOpenGLShader::Vertex, "triangleVertexShader.vert");
    triangleShader.addShaderFromSourceFile(QOpenGLShader::Fragment, "triangleFragmentShader.frag");
    triangleShader.link();
}

void SimpleTriangle::drawTriangle()
{
    glBindVertexArray(VAO);
    triangleShader.bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
}

