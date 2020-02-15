#pragma once
#include<vector>
#include<random>
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
    void focusInEvent(QFocusEvent* event)override;
private:
    bool firstMouse = true, secondMouse = true;

    SimpleTextureBox backgroundPicture;

    void initBoxes();
    void createBoxShader();
    void drawBoxes();
    std::vector<Simple3DBox*> myBoxes;
    QOpenGLShaderProgram boxShader;
    Camera boxCamera{ 800.0f,800.0f };
};
