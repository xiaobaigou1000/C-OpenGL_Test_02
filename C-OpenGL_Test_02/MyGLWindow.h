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
    bool firstMouse = true, secondMouse = true;

    Simple3DBox myBox;
    QOpenGLShaderProgram boxShader;
    Camera boxCamera{ 800.0f,800.0f };
    QOpenGLTexture* boxTexture;
    QOpenGLTexture* boxSpecular;

    glm::vec3 lightPos{ 1.2f, 1.0f, 2.0f };
    Simple3DBox lightBox;
    QOpenGLShaderProgram lightBoxShader;

    std::chrono::steady_clock::time_point lastTimePoint;
    std::chrono::steady_clock::time_point programBeginPoint;

    std::vector<glm::mat4> translateMatrices;
    std::vector<glm::mat4> rotateMatrices;
    std::vector<glm::vec3> rotateAxis;
};
