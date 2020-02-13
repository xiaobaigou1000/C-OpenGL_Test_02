#pragma once
#include<qdebug.h>
#include<array>
#include<qopenglfunctions_4_5_core.h>
#include<qopenglshaderprogram.h>
#include<qopengltexture.h>

class SimpleTextureBox :public QOpenGLFunctions_4_5_Core
{
public:
    unsigned int VAO, VBO, EBO;
    QOpenGLShaderProgram shader;
    QOpenGLTexture* tex;
    void init();
    void draw();

    constexpr static std::array<float, 32> vertices =
    {
     1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
     1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f
    };

    constexpr static std::array<unsigned int, 6> indices =
    {
        0, 1, 3,
        1, 2, 3
    };
};

