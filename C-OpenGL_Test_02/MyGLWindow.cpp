#include "MyGLWindow.h"

MyGLWindow::MyGLWindow(QWidget* parent)
    : QOpenGLWidget(parent)
{
    resize(800, 800);
}

MyGLWindow::~MyGLWindow()
{
}

void MyGLWindow::initializeGL()
{
    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    myTriangle.init();

    for (int i = 0; i < 20; ++i)
    {
        myBoxes.push_back(new Simple3DBox);
    }

    std::default_random_engine dre;
    std::uniform_real_distribution<float> translateDistribution(-3.0f, 3.0f);
    std::uniform_real_distribution<float> angleDistribution(0.0f, 60.0f);
    std::uniform_real_distribution<float> axisDistribution(0.0f, 1.0f);

    auto rotateMat = glm::rotate(glm::mat4(1.0f), glm::radians(angleDistribution(dre)), glm::vec3(axisDistribution(dre), axisDistribution(dre), axisDistribution(dre)));
    auto translateMat = glm::translate(glm::mat4(1.0f), glm::vec3(translateDistribution(dre), translateDistribution(dre), translateDistribution(dre)));

    for (auto& i : myBoxes)
    {
        rotateMat = glm::rotate(glm::mat4(1.0f), glm::radians(angleDistribution(dre)), glm::vec3(axisDistribution(dre), axisDistribution(dre), axisDistribution(dre)));
        translateMat = glm::translate(glm::mat4(1.0f), glm::vec3(translateDistribution(dre), translateDistribution(dre), translateDistribution(dre)));

        i->init();
        i->resetRotateMat(rotateMat);
        i->resetTranslateMat(translateMat);
    }

    int maxVertexAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
    qInfo() << "GL_MAX_VERTEX_ATTRIBS : " << maxVertexAttribs;
}

void MyGLWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    myTriangle.draw();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    for (auto& i : myBoxes)
    {
        i->draw();
    }
    update();
}

void MyGLWindow::resizeGL(int w, int h)
{
    myTriangle.resize(w, h);
    for (auto& i : myBoxes)
    {
        i->resize(w, h);
    }
}
