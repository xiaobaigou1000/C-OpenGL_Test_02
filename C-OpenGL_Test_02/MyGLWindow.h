#pragma once
#include<qdebug.h>
#include<qopenglfunctions_4_5_core.h>
#include<qopenglwidget.h>
#include"SimpleTriangle.h"

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
    SimpleTriangle myTriangle;
};
