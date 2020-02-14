#include "SimpleTextureBox.h"
#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp>

void SimpleTextureBox::init()
{
    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();

    glCreateBuffers(1, &VBO);
    glNamedBufferData(VBO, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glCreateBuffers(1, &EBO);
    glNamedBufferData(EBO, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);

    glCreateVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    shader.create();
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, "triangleVertexShader.vert");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, "triangleFragmentShader.frag");
    shader.link();

    shader.bind();
    glm::mat4 rotateMat = glm::rotate(glm::mat4(1.0f), 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 translateMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.3f, 0.5f, 0.0f));
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 1.0f, 1.0f));
    glUniformMatrix4fv(shader.uniformLocation("translateMat"), 1, GL_FALSE, glm::value_ptr(rotateMat));
    orthogonalMat = glm::ortho(0.0f, 2.0f, 0.0f, 2.0f, -0.5f, 0.5f);
    glUniformMatrix4fv(shader.uniformLocation("translateMat"), 1, GL_FALSE, glm::value_ptr(orthogonalMat));

    tex = new QOpenGLTexture(QImage(QString("./images/texture_test.jpg")).mirrored());
}

void SimpleTextureBox::draw()
{
    glBindVertexArray(VAO);
    shader.bind();
    glActiveTexture(GL_TEXTURE0);
    tex->bind(GL_TEXTURE_2D);
    glUniform1i(shader.uniformLocation("tex"), 0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void SimpleTextureBox::resize(int w, int h)
{
    orthogonalMat = glm::ortho(-1.0f, 1.0f, -1.0f, -1.0f + (2.0f * (float(h)/float(w))), -0.5f, 0.5f);
    shader.bind();
    glUniformMatrix4fv(shader.uniformLocation("translateMat"), 1, GL_FALSE, glm::value_ptr(orthogonalMat));
}
