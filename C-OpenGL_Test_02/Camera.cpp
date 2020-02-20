#include "Camera.h"

#include<gtc/matrix_transform.hpp>
#include<cmath>

using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::steady_clock;
using glm::vec3;
using glm::cross;
using glm::normalize;
using glm::perspective;
using glm::lookAt;
using glm::radians;

Camera::Camera(float windowWidth, float windowHeight)
    :windowWidth(windowWidth), windowHeight(windowHeight)
{

}

void Camera::resizeCamera(int w, int h)
{
    windowWidth = static_cast<float>(w);
    windowHeight = static_cast<float>(h);
    projectionMat = perspective(FOV, windowWidth / windowHeight, 0.1f, 5.0f);
}

void Camera::caculateCamera()
{
    auto currentTime = steady_clock::now();
    float intervalTime = duration_cast<duration<float>>(currentTime - lastFrame).count();
    if (keyW)
    {
        position += front * intervalTime * speed;
    }
    if (keyS)
    {
        position -= front * intervalTime * speed;
    }
    if (keyA)
    {
        position -= normalize(cross(front, worldUp)) * intervalTime * speed;
    }
    if (keyD)
    {
        position += normalize(cross(front, worldUp)) * intervalTime * speed;
    }
    lastFrame = currentTime;
}

glm::mat4 Camera::viewProjectionMat()
{
    return projectionMat * lookAt(position, position + front, worldUp);
}

void Camera::setKeyW(bool current)
{
    keyW = current;
}

void Camera::setKeyS(bool current)
{
    keyS = current;
}

void Camera::setKeyA(bool current)
{
    keyA = current;
}

void Camera::setKeyD(bool current)
{
    keyD = current;
}

void Camera::processMouseMovement(float w, float h)
{
    yaw += w / windowWidth * mouseSensitivity;
    pitch -= h / windowHeight * mouseSensitivity;
    if (pitch > 1.57f)
        pitch = 1.57f;
    if (pitch < -1.57f)
        pitch = -1.57f;

    front.y = sinf(pitch);
    front.x = cosf(pitch) * cosf(yaw);
    front.z = cosf(pitch) * sinf(yaw);
}
