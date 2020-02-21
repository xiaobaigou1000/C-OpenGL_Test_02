#include "MyGLWindow.h"

#include<QKeyEvent>
#include<cmath>

using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using glm::mat4;
using glm::vec3;
using glm::translate;
using glm::scale;
using glm::value_ptr;
using glm::rotate;
using glm::radians;
using glm::normalize;
using glm::perspective;
using glm::lookAt;

MyGLWindow::MyGLWindow(QWidget* parent)
    : QOpenGLWidget(parent)
{
    resize(800, 800);
    //setWindowFlags(Qt::FramelessWindowHint);
    setCursor(Qt::BlankCursor);
    setMouseTracking(true);
}

MyGLWindow::~MyGLWindow()
{
}

void MyGLWindow::initializeGL()
{
    programBeginPoint = lastTimePoint = std::chrono::steady_clock::now();
    std::default_random_engine dre(std::chrono::system_clock::now().time_since_epoch().count());

    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    testModel.loadModel("./models/nanosuit/nanosuit.obj");
    testModel.init();
    modelShader.create();
    modelShader.addShaderFromSourceFile(QOpenGLShader::Vertex, "./shaders/model.vert");
    modelShader.addShaderFromSourceFile(QOpenGLShader::Fragment, "./shaders/model.frag");
    modelShader.link();

    //light box
    lightBoxShader.create();
    lightBoxShader.addShaderFromSourceFile(QOpenGLShader::Vertex, "./shaders/boxShader.vert");
    lightBoxShader.addShaderFromSourceFile(QOpenGLShader::Fragment, "./shaders/lightBox.frag");
    lightBoxShader.link();
    lightBox.init();
    lightBox.resetScaleMat(scale(mat4(1.0f), vec3{ 0.2f }));
    std::normal_distribution<float> lightPosDistribution{ 0.0f, 2.0f };
    for (int i = 0; i < 4; ++i)
    {
        pointLightColor.push_back({ 1.0f,1.0f,1.0f });
    }
    lightPos.push_back(vec3(0.0f, -1.7f, 0.4f));
    lightPos.push_back(vec3(-0.7f, -0.8f, 0.0f));
    lightPos.push_back(vec3(0.0f, 0.4f, 0.7f));
    lightPos.push_back(vec3(-0.4f, 1.7f, 0.0f));

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &texColorBufferFBO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texColorBufferFBO);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 800, 0, GL_RGB, GL_UNSIGNED_INT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBufferFBO, 0);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 800);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
        qInfo() << "frame buffer init success.";
    }
    else
    {
        qInfo() << "frame buffer init failed.";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MyGLWindow::paintGL()
{
    mainCamera.caculateCamera();
    auto currentTime = std::chrono::steady_clock::now();
    auto passedDuration = duration_cast<duration<float>>(currentTime - lastTimePoint).count();
    auto timeFromBeginPoint = duration_cast<duration<float>>(currentTime - programBeginPoint).count();
    lastTimePoint = currentTime;

    //light box
    vec3 lightColor{ 1.0f, 1.0f, 1.0f };
    lightColor.r = 0.5f + 0.5f * sin(timeFromBeginPoint);
    lightColor.g = 0.5f + 0.5f * cos(timeFromBeginPoint);
    lightColor.b = 1.0f - lightColor.r;
    vec3 diffuseColor = lightColor * vec3{ 0.7f };
    vec3 ambientColor = diffuseColor * vec3{ 0.3f };

    lightBoxShader.bind();
    lightBox.bind();
    glUniform3fv(lightBoxShader.uniformLocation("lightColor"), 1, value_ptr(lightColor));
    for (int i = 0; i < lightPos.size(); ++i)
    {
        pointLightColor[i].r = 0.5f + 0.5f * sin(0.5f + timeFromBeginPoint + 0.5f * i);
        pointLightColor[i].g = 0.5f + 0.5f * cos(0.5f + timeFromBeginPoint + 0.5f * i);
        pointLightColor[i].b = 1.0f - pointLightColor[i].r;
        glUniform3fv(lightBoxShader.uniformLocation("lightColor"), 1, value_ptr(pointLightColor[i]));

        mat4 lightModel = translate(mat4{ 1.0f }, lightPos[i]) * lightBox.scaleMat;
        glUniformMatrix4fv(lightBoxShader.uniformLocation("MVP"), 1, GL_FALSE, value_ptr(mainCamera.viewProjectionMat() * lightModel));
        lightBox.draw();
    }

    //model
    modelShader.bind();
    vec3 position;
    position.x = 5.0f * sin(timeFromBeginPoint);
    position.z = 5.0f * cos(timeFromBeginPoint);
    mat4 translateMat = translate(mat4{ 1.0f }, vec3(0.0f, -1.75f, 0.0f));
    mat4 scaleMat = scale(mat4{ 1.0f }, vec3(0.2f, 0.2f, 0.2f));
    mat4 modelMat = translateMat * scaleMat;

    setLightVariableForShader(ambientColor, diffuseColor);
    glUniform1f(modelShader.uniformLocation("material.shininess"), 32.0f);
    glUniformMatrix4fv(modelShader.uniformLocation("MVP"), 1, GL_FALSE, value_ptr(mainCamera.viewProjectionMat() * modelMat));
    glUniformMatrix4fv(modelShader.uniformLocation("modelMat"), 1, GL_FALSE, value_ptr(modelMat));
    glUniform3fv(modelShader.uniformLocation("viewPos"), 1, value_ptr(mainCamera.position));
    testModel.draw(&modelShader);

    update();
}

void MyGLWindow::resizeGL(int w, int h)
{
    mainCamera.resizeCamera(w, h);
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
    mainCamera.processMouseMovement(xAxisMove, yAxisMove);
    QCursor myCursor = cursor();
    myCursor.setPos(mapToGlobal({ width() / 2, height() / 2 }));
    setCursor(myCursor);
}

void MyGLWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        close();
    if (event->key() == Qt::Key_W)
        mainCamera.setKeyW(true);
    if (event->key() == Qt::Key_S)
        mainCamera.setKeyS(true);
    if (event->key() == Qt::Key_A)
        mainCamera.setKeyA(true);
    if (event->key() == Qt::Key_D)
        mainCamera.setKeyD(true);
}

void MyGLWindow::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_W)
        mainCamera.setKeyW(false);
    if (event->key() == Qt::Key_S)
        mainCamera.setKeyS(false);
    if (event->key() == Qt::Key_A)
        mainCamera.setKeyA(false);
    if (event->key() == Qt::Key_D)
        mainCamera.setKeyD(false);
}

void MyGLWindow::setLightVariableForShader(vec3 ambientColor, vec3 diffuseColor)
{
    vec3 spotLightColor{ 1.0f, 1.0f, 1.0f };
    vec3 spotDiffuseColor = spotLightColor * vec3{ 0.7f };
    vec3 spotAmbientColor = spotDiffuseColor * vec3{ 0.3f };

    glUniform3f(modelShader.uniformLocation("dirlight.direction"), -0.2f, -1.0f, -0.3f);
    glUniform3fv(modelShader.uniformLocation("dirlight.ambient"), 1, value_ptr(ambientColor));
    glUniform3fv(modelShader.uniformLocation("dirlight.diffuse"), 1, value_ptr(diffuseColor));
    glUniform3f(modelShader.uniformLocation("dirlight.specular"), 1.0f, 1.0f, 1.0f);

    for (int i = 0; i < 4; ++i)
    {
        vec3 pointLightDiffuseColor = pointLightColor[i] * vec3{ 0.7f };
        vec3 pointLightAmbientColor = pointLightDiffuseColor * vec3{ 0.1f };

        QString lightName = QString::fromStdString("pointlights[" + std::to_string(i) + "].");
        glUniform3fv(modelShader.uniformLocation(lightName + "position"), 1, value_ptr(lightPos[i]));
        glUniform1f(modelShader.uniformLocation(lightName + "constant"), 1.0f);
        glUniform1f(modelShader.uniformLocation(lightName + "linear"), 0.09f);
        glUniform1f(modelShader.uniformLocation(lightName + "quadratic"), 0.032f);
        glUniform3fv(modelShader.uniformLocation(lightName + "ambient"), 1, value_ptr(pointLightAmbientColor));
        glUniform3fv(modelShader.uniformLocation(lightName + "diffuse"), 1, value_ptr(pointLightDiffuseColor));
        glUniform3f(modelShader.uniformLocation(lightName + "specular"), 1.0f, 1.0f, 1.0f);
    }

    glUniform3fv(modelShader.uniformLocation("spotlight.position"), 1, value_ptr(mainCamera.position));
    glUniform3fv(modelShader.uniformLocation("spotlight.direction"), 1, value_ptr(mainCamera.front));
    glUniform1f(modelShader.uniformLocation("spotlight.cutOff"), cosf(radians(12.5f)));
    glUniform1f(modelShader.uniformLocation("spotlight.outerCutOff"), cosf(radians(17.5f)));
    glUniform3fv(modelShader.uniformLocation("spotlight.ambient"), 1, value_ptr(spotAmbientColor));
    glUniform3fv(modelShader.uniformLocation("spotlight.diffuse"), 1, value_ptr(spotDiffuseColor));
    glUniform3f(modelShader.uniformLocation("spotlight.specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(modelShader.uniformLocation("spotlight.constant"), 1.0f);
    glUniform1f(modelShader.uniformLocation("spotlight.linear"), 0.09f);
    glUniform1f(modelShader.uniformLocation("spotlight.quadratic"), 0.032f);
}
