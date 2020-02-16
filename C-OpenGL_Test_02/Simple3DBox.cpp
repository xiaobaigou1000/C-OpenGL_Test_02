#include "Simple3DBox.h"

using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using glm::mat4;
using glm::vec3;
using glm::translate;
using glm::rotate;
using glm::radians;

void Simple3DBox::init(QOpenGLShaderProgram* newShader)
{
    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();
    shader = newShader;

    programBeginPoint = lastTimePoint = steady_clock::now();

    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glNamedBufferData(VBO, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    translateMat = mat4(1.0f);
    scaleMat = mat4(1.0f);
    //rotateMat = rotate(mat4(1.0f), radians(55.0f), vec3(1.0f, 1.0f, 0.0f));

    tex = new QOpenGLTexture(QImage("./images/texture_test.jpg").mirrored());
}

glm::mat4 Simple3DBox::getModelMat()
{
    return translateMat * rotateMat * scaleMat;
}

void Simple3DBox::draw(const mat4& viewProjectionMat)
{
    shader->bind();
    drawWithoutSettingShader(viewProjectionMat);
}

void Simple3DBox::drawWithoutSettingShader(const mat4& viewProjectionMat)
{
    glBindVertexArray(VAO);
    auto currentTime = steady_clock::now();
    auto dur = duration_cast<duration<float, std::ratio<1>>>(currentTime - lastTimePoint);
    lastTimePoint = currentTime;
    rotateMat = rotate(rotateMat, radians(dur.count() * 30.0f), rotateDirection);
    glUniformMatrix4fv(shader->uniformLocation("MVP"), 1, GL_FALSE, value_ptr(viewProjectionMat * getModelMat()));

    glActiveTexture(GL_TEXTURE0);
    tex->bind(GL_TEXTURE_2D);
    glUniform1i(shader->uniformLocation("tex"), 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
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
