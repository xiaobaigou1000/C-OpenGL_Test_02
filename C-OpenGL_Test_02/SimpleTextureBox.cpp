#include "SimpleTextureBox.h"

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    shader.create();
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, "triangleVertexShader.vert");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, "triangleFragmentShader.frag");
    shader.link();

    tex=new QOpenGLTexture(QImage(QString("./images/texture_test.jpg")).mirrored());
}

void SimpleTextureBox::draw()
{
    using namespace std::chrono_literals;
    glBindVertexArray(VAO);
    shader.bind();
    glActiveTexture(GL_TEXTURE0);
    tex->bind(GL_TEXTURE_2D);
    glUniform1i(shader.uniformLocation("tex"), 0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
