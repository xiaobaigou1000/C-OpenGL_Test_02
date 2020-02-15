#include "Simple3DBox.h"

void Simple3DBox::init(QOpenGLShaderProgram* newShader)
{
    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();
    shader = newShader;

    programBeginPoint = lastTimePoint = std::chrono::steady_clock::now();

    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glNamedBufferData(VBO, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    translateMat = glm::mat4(1.0f);
    scaleMat = glm::mat4(1.0f);
    rotateMat = glm::rotate(glm::mat4(1.0f), glm::radians(55.0f), glm::vec3(1.0f, 1.0f, 0.0f));

    tex = new QOpenGLTexture(QImage("./images/texture_test.jpg").mirrored());
}

void Simple3DBox::draw(const glm::mat4& viewProjectionMat)
{
    shader->bind();
    drawWithoutSettingShader(viewProjectionMat);
}

void Simple3DBox::drawWithoutSettingShader(const glm::mat4& viewProjectionMat)
{
    glBindVertexArray(VAO);
    auto currentTime = std::chrono::steady_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(currentTime - lastTimePoint);
    lastTimePoint = currentTime;
    rotateMat = glm::rotate(rotateMat, glm::radians(dur.count() * 30.0f), rotateDirection);
    glUniformMatrix4fv(shader->uniformLocation("MVP"), 1, GL_FALSE, value_ptr(viewProjectionMat * translateMat * rotateMat * scaleMat));

    glActiveTexture(GL_TEXTURE0);
    tex->bind(GL_TEXTURE_2D);
    glUniform1i(shader->uniformLocation("tex"), 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
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

void Simple3DBox::resetRotateDirection(const glm::vec3& newDirection)
{
    rotateDirection = newDirection;
}

void Simple3DBox::resetShader(QOpenGLShaderProgram* newShader)
{
    shader = newShader;
}
