#include "Simple3DBox.h"

using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using glm::mat4;
using glm::vec3;
using glm::translate;
using glm::rotate;
using glm::radians;

void Simple3DBox::init()
{
    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();
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
}

void Simple3DBox::bind()
{
    glBindVertexArray(VAO);
}

void Simple3DBox::draw()
{
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

glm::mat4 Simple3DBox::getModelMat()
{
    return translateMat * rotateMat * scaleMat;
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
