#pragma once
#include<qdebug.h>
#include<array>
#include<qopenglfunctions_4_5_core.h>
#include<qopenglshaderprogram.h>

class SimpleTriangle :public QOpenGLFunctions_4_5_Core
{
public:
    SimpleTriangle();
    unsigned int VAO, VBO, EBO;
    QOpenGLShaderProgram shader;
    void init();
    void draw();

    constexpr static std::array<float, 18> vertices =
    {
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f
    };

    constexpr static std::array<unsigned int, 6> indices =
    {
        0, 1, 3,
        1, 2, 3
    };
};

