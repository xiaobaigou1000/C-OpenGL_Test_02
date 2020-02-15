#pragma once
#include<vector>
#include<random>
#include<qdebug.h>
#include<qopenglfunctions_4_5_core.h>
#include<qopenglwidget.h>
#include"SimpleTextureBox.h"
#include"Simple3DBox.h"

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
    SimpleTextureBox backgroundPicture;

    void initBoxes();
    void createBoxShader();
    void drawBoxes();
    void resizeBoxes(int w, int h);
    std::vector<Simple3DBox*> myBoxes;
    QOpenGLShaderProgram boxShader;
};
