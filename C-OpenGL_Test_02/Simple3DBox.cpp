#include "Simple3DBox.h"

void Simple3DBox::init()
{
    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();

    using glm::mat4;
    using glm::vec3;
    using glm::translate;
    using glm::rotate;
    using glm::scale;
    using glm::value_ptr;
    using glm::perspective;
    using glm::radians;

    programBeginPoint = lastTimePoint = sc.now();

    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glNamedBufferData(VBO, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    shader.create();
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, "boxShader.vert");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, "boxShader.frag");
    shader.link();

    shader.bind();
    translateMat = mat4(1.0f);
    scaleMat = mat4(1.0f);
    rotateMat = rotate(mat4(1.0f), radians(55.0f), vec3(1.0f, 1.0f, 0.0f));
    view = translate(mat4(1.0f), vec3(0.0f, 0.0f, -10.0f));
    projection = perspective(radians(50.0f), 800.0f / 800.0f, 0.1f, 100.0f);
    mat4 mvp = projection * view * translateMat * rotateMat * scaleMat;
    glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, value_ptr(mvp));

    tex = new QOpenGLTexture(QImage("./images/texture_test.jpg").mirrored());
}

void Simple3DBox::draw()
{

    glBindVertexArray(VAO);
    shader.bind();
    auto currentTime = sc.now();
    auto dur = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(currentTime - lastTimePoint);
    lastTimePoint = currentTime;
    rotateMat = glm::rotate(rotateMat, glm::radians(dur.count() * 30.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, glm::value_ptr(projection * view * translateMat * rotateMat * scaleMat));

    glActiveTexture(GL_TEXTURE0);
    tex->bind(GL_TEXTURE_2D);
    glUniform1i(shader.uniformLocation("tex"), 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Simple3DBox::resize(int w, int h)
{
    projection = glm::perspective(glm::radians(50.0f), static_cast<float>(w) / static_cast<float>(h), 0.1f, 100.0f);
    shader.bind();
    glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, value_ptr(projection * view * translateMat * rotateMat * scaleMat));
}

void Simple3DBox::resetTranslateMat(const glm::mat4& newTranslateMat)
{
    translateMat = newTranslateMat;
}

void Simple3DBox::resetScaleMat(const glm::mat4& newScaleMat)
{
    scaleMat = newScaleMat;
}

void Simple3DBox::resetRotateMat(const glm::mat4& newRotateMat)
{
    rotateMat = newRotateMat;
}
