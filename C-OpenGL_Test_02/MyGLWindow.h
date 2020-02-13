#pragma once

#include<qopenglwidget.h>
#include<qopenglfunctions_4_5_core.h>
#include<qopenglshaderprogram.h>
#include<array>

class MyGLWindow : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

public:
    MyGLWindow(QWidget* parent = nullptr);
    ~MyGLWindow();

    void initializeGL()override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
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
