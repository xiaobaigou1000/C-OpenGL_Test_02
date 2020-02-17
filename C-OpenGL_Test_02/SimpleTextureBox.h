#pragma once
#include<qdebug.h>
#include<array>
#include<qopenglfunctions_4_5_core.h>
#include<qopenglshaderprogram.h>
#include<qopengltexture.h>
#include<glm.hpp>

class SimpleTextureBox :public QOpenGLFunctions_4_5_Core
{
public:
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    QOpenGLShaderProgram shader;
    QOpenGLTexture* tex = nullptr;
    void init();
    void draw();

    glm::mat4 scaleMat{ 1.0f };
    glm::mat4 orthogonalMat{ 1.0f };
    void resize(int w, int h);

    constexpr static std::array<float, 32> vertices =
    {
     1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
     1.0f,  0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
     0.0f,  0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
     0.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f
    };

    constexpr static std::array<unsigned int, 6> indices =
    {
        0, 1, 3,
        1, 2, 3
    };
};

