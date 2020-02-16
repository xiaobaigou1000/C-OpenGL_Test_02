#include "MyGLWindow.h"

#include<QKeyEvent>

MyGLWindow::MyGLWindow(QWidget* parent)
    : QOpenGLWidget(parent)
{
    resize(800, 800);
    setCursor(Qt::BlankCursor);
    setMouseTracking(true);
}

MyGLWindow::~MyGLWindow()
{
}

void MyGLWindow::initializeGL()
{
    using glm::mat4;
    using glm::vec3;
    using glm::translate;
    using glm::scale;
    using glm::value_ptr;

    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    backgroundPicture.init();
    createBoxShader();
    initBoxes();

    lightBoxShader.create();
    lightBoxShader.addShaderFromSourceFile(QOpenGLShader::Vertex, "boxShader.vert");
    lightBoxShader.addShaderFromSourceFile(QOpenGLShader::Fragment, "lightBox.frag");
    lightBoxShader.link();
    lightBox.init(&lightBoxShader);
    lightBox.resetTranslateMat(translate(mat4(1.0f), vec3{ 0.2f }));
    lightBox.resetScaleMat(scale(mat4(1.0f), lightPos));

    int maxVertexAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
    qInfo() << "GL_MAX_VERTEX_ATTRIBS : " << maxVertexAttribs;
}

void MyGLWindow::paintGL()
{

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); !!Qt already cleaned last frame, no need for manually clean.
    glDisable(GL_DEPTH_TEST);
    backgroundPicture.draw();

    boxCamera.caculateCamera();
    drawBoxes();

    lightBox.draw(boxCamera.viewProjectionMat());

    update();
}

void MyGLWindow::resizeGL(int w, int h)
{
    backgroundPicture.resize(w, h);
    boxCamera.resizeCamera(w, h);
}

void MyGLWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (!rect().contains(event->pos()))
        return;
    if (firstMouse)
    {
        QCursor myCursor = cursor();
        myCursor.setPos(mapToGlobal({ width() / 2, height() / 2 }));
        setCursor(myCursor);
        firstMouse = false;
        return;
    }
    if (secondMouse)
    {
        secondMouse = false;
        return;
    }
    float xAxisMove = event->x() - width() / 2;
    float yAxisMove = event->y() - height() / 2;
    boxCamera.processMouseMovement(xAxisMove, yAxisMove);
    QCursor myCursor = cursor();
    myCursor.setPos(mapToGlobal({ width() / 2, height() / 2 }));
    setCursor(myCursor);
}

void MyGLWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        close();
    if (event->key() == Qt::Key_W)
        boxCamera.setKeyW(true);
    if (event->key() == Qt::Key_S)
        boxCamera.setKeyS(true);
    if (event->key() == Qt::Key_A)
        boxCamera.setKeyA(true);
    if (event->key() == Qt::Key_D)
        boxCamera.setKeyD(true);
}

void MyGLWindow::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_W)
        boxCamera.setKeyW(false);
    if (event->key() == Qt::Key_S)
        boxCamera.setKeyS(false);
    if (event->key() == Qt::Key_A)
        boxCamera.setKeyA(false);
    if (event->key() == Qt::Key_D)
        boxCamera.setKeyD(false);
}

void MyGLWindow::focusInEvent(QFocusEvent* event)
{
    return;
}

void MyGLWindow::initBoxes()
{
    using glm::mat4;
    using glm::vec3;
    using glm::rotate;
    using glm::radians;

    for (int i = 0; i < 20; ++i)
    {
        myBoxes.push_back(new Simple3DBox);
    }

    std::default_random_engine dre;
    std::uniform_real_distribution<float> translateDistribution(-3.0f, 3.0f);
    std::uniform_real_distribution<float> angleDistribution(-60.0f, 60.0f);
    std::uniform_real_distribution<float> axisDistribution(0.0f, 1.0f);

    auto rotateMat = rotate(mat4(1.0f), radians(angleDistribution(dre)), vec3(axisDistribution(dre), axisDistribution(dre), axisDistribution(dre)));
    auto translateMat = translate(mat4(1.0f), vec3(translateDistribution(dre), translateDistribution(dre), translateDistribution(dre)));
    for (auto& i : myBoxes)
    {
        vec3 rotateDirection{ axisDistribution(dre), axisDistribution(dre), axisDistribution(dre) };
        rotateMat = rotate(mat4(1.0f), radians(angleDistribution(dre)), rotateDirection);
        translateMat = translate(mat4(1.0f), vec3(translateDistribution(dre), translateDistribution(dre), translateDistribution(dre)));

        i->init(&boxShader);
        i->resetRotateMat(rotateMat);
        i->resetTranslateMat(translateMat);
        i->resetRotateDirection(rotateDirection);
    }
}

void MyGLWindow::createBoxShader()
{
    boxShader.create();
    boxShader.addShaderFromSourceFile(QOpenGLShader::Vertex, "boxShader.vert");
    boxShader.addShaderFromSourceFile(QOpenGLShader::Fragment, "boxShader.frag");
    boxShader.link();

    boxShader.bind();
    glUniform3f(boxShader.uniformLocation("objectColor"), 1.0f, 0.5f, 0.31f);
    glUniform3f(boxShader.uniformLocation("lightColor"), 1.0f, 1.0f, 1.0f);
}

void MyGLWindow::drawBoxes()
{
    glEnable(GL_DEPTH_TEST);
    boxShader.bind();
    glm::mat4 viewProjectionMat = boxCamera.viewProjectionMat();

    for (auto& i : myBoxes)
    {
        i->drawWithoutSettingShader(viewProjectionMat);
    }
}
