#pragma once
#include<vector>
#include<random>
#include<chrono>
#include<qdebug.h>
#include<qopenglfunctions_4_5_core.h>
#include<qopenglwidget.h>
#include"SimpleTextureBox.h"
#include"Simple3DBox.h"
#include"Camera.h"
#include"Model.h"

class MyGLWindow : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

public:
    MyGLWindow(QWidget* parent = nullptr);
    ~MyGLWindow();

    void initializeGL()override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mouseMoveEvent(QMouseEvent* event)override;
    void keyPressEvent(QKeyEvent* event)override;
    void keyReleaseEvent(QKeyEvent* event)override;
private:
    void setLightVariableForShader(glm::vec3 ambientColor, glm::vec3 diffuseColor);

    bool firstMouse = true, secondMouse = true;

    std::vector<glm::vec3> pointLightColor;
    std::vector<glm::vec3> lightPos;
    Simple3DBox lightBox;
    QOpenGLShaderProgram lightBoxShader;

    QOpenGLShaderProgram singleColorShader;

    Model testModel;
    QOpenGLShaderProgram modelShader;
    Camera boxCamera{ 800.0f,800.0f };
    std::chrono::steady_clock::time_point lastTimePoint;
    std::chrono::steady_clock::time_point programBeginPoint;

    std::vector<glm::vec3> vegetation;
    unsigned int vegetationVAO, vegetationVBO, vegetationEBO;
    QOpenGLShaderProgram vegetationShader;
    QOpenGLTexture* vegetationTex;

    constexpr static std::array<float, 20> vegetationVertices
    {
        0,0,0,0,0,
        1,1,0,1,1,
        0,1,0,0,1,
        1,0,0,1,0
    };

    constexpr static std::array<unsigned int, 233> vegetationIndices
    {
        0,1,2,
        0,3,1
    };
};
