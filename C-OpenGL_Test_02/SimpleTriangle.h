#pragma once
#include<array>
#include<qopenglfunctions_4_5_core.h>
#include<qopenglshaderprogram.h>

class SimpleTriangle :public QOpenGLFunctions_4_5_Core
{
public:
    SimpleTriangle();
    unsigned int triangleVAO, triangleVBO, triangleEBO;
    QOpenGLShaderProgram triangleShader;
    void initTriangle();
    void drawTriangle();

    constexpr static std::array<float, 9> triangleVertices =
    {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
};

