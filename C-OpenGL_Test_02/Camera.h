#pragma once
#include<glm.hpp>
#include<chrono>

class Camera
{
public:
    Camera(float windowWidth, float windowHeight);

    glm::vec3 position{ 0.0f,0.0f,-3.0f };
    glm::vec3 front{ 0.0f,0.0f,1.0f };
    glm::vec3 worldUp{ 0.0f,1.0f,0.0f };
    float yaw = 0.0f, pitch = 0.0f;

    float FOV = 45.0f;
    float nearPlane = 0.1f, farPlane = 100.0f;
    float windowWidth, windowHeight;
    glm::mat4 projectionMat{ 1.0f };
    void resizeCamera(int w, int h);

    float speed = 2.0f;
    float mouseSensitivity = 5.0f;

    std::chrono::steady_clock::time_point lastFrame = std::chrono::steady_clock::now();

    bool keyW = false, keyS = false, keyA = false, keyD = false;
    void caculateCamera();
    glm::mat4 viewProjectionMat();
    void setKeyW(bool current);
    void setKeyS(bool current);
    void setKeyA(bool current);
    void setKeyD(bool current);
    void processMouseMovement(float w, float h);
};
