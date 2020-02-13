#include "SimpleTriangle.h"
#include<cmath>

SimpleTriangle::SimpleTriangle()
{
}

void SimpleTriangle::init()
{
    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();
    startTimePoint = sc.now();

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
    auto colorBase = (sc.now() - startTimePoint) % 10s;
    float cur = 0.5f + 0.5f * sinf(std::chrono::duration_cast<std::chrono::seconds>(colorBase).count()/10.0f);
    glUniform4f(shader.uniformLocation("color"), cur, 0.0f, 1.0f-cur, cur);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
}

