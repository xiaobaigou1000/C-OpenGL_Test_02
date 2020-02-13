#include "SimpleTriangle.h"
#include<cmath>

SimpleTriangle::SimpleTriangle()
{
}

void SimpleTriangle::init()
{
    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();

    glCreateBuffers(1, &VBO);
    glNamedBufferData(VBO, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glCreateBuffers(1, &EBO);
    glNamedBufferData(EBO, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);

    glCreateVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    shader.create();
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, "triangleVertexShader.vert");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, "triangleFragmentShader.frag");
    shader.link();
}

void SimpleTriangle::draw()
{
    using namespace std::chrono_literals;
    glBindVertexArray(VAO);
    shader.bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

