#include "Simple3DBox.h"

using glm::mat4;
using glm::vec3;
using glm::translate;
using glm::rotate;
using glm::scale;
using glm::value_ptr;
using glm::perspective;
using glm::radians;

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

    translateMat = mat4(1.0f);
    scaleMat = mat4(1.0f);
    rotateMat = rotate(mat4(1.0f), radians(55.0f), vec3(1.0f, 1.0f, 0.0f));
    view = translate(mat4(1.0f), vec3(0.0f, 0.0f, -10.0f));
    projection = perspective(radians(50.0f), 800.0f / 800.0f, 0.1f, 100.0f);

    tex = new QOpenGLTexture(QImage("./images/texture_test.jpg").mirrored());
}

void Simple3DBox::draw()
{
    shader->bind();
    drawWithoutSettingShader();
}

void Simple3DBox::drawWithoutSettingShader()
{
    glBindVertexArray(VAO);
    auto currentTime = std::chrono::steady_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(currentTime - lastTimePoint);
    lastTimePoint = currentTime;
    rotateMat = rotate(rotateMat, radians(dur.count() * 30.0f), rotateDirection);
    glUniformMatrix4fv(shader->uniformLocation("MVP"), 1, GL_FALSE, value_ptr(projection * view * translateMat * rotateMat * scaleMat));

    glActiveTexture(GL_TEXTURE0);
    tex->bind(GL_TEXTURE_2D);
    glUniform1i(shader->uniformLocation("tex"), 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Simple3DBox::resize(int w, int h)
{
    projection = perspective(radians(50.0f), static_cast<float>(w) / static_cast<float>(h), 0.1f, 100.0f);
}

void Simple3DBox::resetTranslateMat(const mat4& newTranslateMat)
{
    translateMat = newTranslateMat;
}

void Simple3DBox::resetScaleMat(const mat4& newScaleMat)
{
    scaleMat = newScaleMat;
}

void Simple3DBox::resetRotateMat(const mat4& newRotateMat)
{
    rotateMat = newRotateMat;
}

void Simple3DBox::resetRotateDirection(const vec3& newDirection)
{
    rotateDirection = newDirection;
}

void Simple3DBox::resetShader(QOpenGLShaderProgram* newShader)
{
    shader = newShader;
}
