#include "MyGLWindow.h"

#include<qimage.h>
#include<QKeyEvent>
#include<cmath>
#include<memory>

using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using glm::mat4;
using glm::mat3;
using glm::vec3;
using glm::vec2;
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
    resize(1920, 1080);
    setWindowFlags(Qt::FramelessWindowHint);
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
    glDepthFunc(GL_LEQUAL);

    //code here
    instanceModel.planet.loadModel("./models/planet/planet.obj");
    instanceModel.rocks.loadModel("./models/rock/rock.obj");
    instanceModel.planet.init();
    instanceModel.rocks.init();

    instanceModel.shader.create();
    instanceModel.shader.addShaderFromSourceFile(QOpenGLShader::Vertex, "./shaders/instanceModel.vert");
    instanceModel.shader.addShaderFromSourceFile(QOpenGLShader::Fragment, "./shaders/instanceModel.frag");
    instanceModel.shader.link();

    instanceModel.planet.setAdditionalVertexAttribute([this] {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
        this->glVertexAttrib4fv(4, value_ptr(model[0]));
        this->glVertexAttrib4fv(5, value_ptr(model[1]));
        this->glVertexAttrib4fv(6, value_ptr(model[2]));
        this->glVertexAttrib4fv(7, value_ptr(model[3]));
        });

    float radius = 150.0f;
    std::uniform_real_distribution<float> offset(-25.0f, 25.0f);
    std::uniform_real_distribution<float> scaleDist(0.05f, 0.25f);
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14f);
    for (size_t i = 0; i < 100000; i++)
    {
        float angle = angleDist(dre);
        float x = sin(angle) * radius + offset(dre);
        float z = cos(angle) * radius + offset(dre);
        float y = 0.4f * offset(dre);
        mat4 translateMat = translate(mat4(1.0f), vec3(x, y, z));
        mat4 scaleMat = scale(mat4(1.0f), vec3(scaleDist(dre)));
        mat4 rotateMat = rotate(mat4(1.0f), angleDist(dre), vec3(0.4f, 0.6f, 0.8f));
        mat4 modelMat = translateMat * rotateMat * scaleMat;
        instanceModel.rocksModelMats.push_back(std::move(modelMat));
    }

    glGenBuffers(1, &instanceModel.modelMatVbo);
    glBindBuffer(GL_ARRAY_BUFFER, instanceModel.modelMatVbo);
    glBufferStorage(GL_ARRAY_BUFFER, instanceModel.rocksModelMats.size() * sizeof(mat4), instanceModel.rocksModelMats.data(), 0);
    unsigned int vboForCapture = instanceModel.modelMatVbo;
    instanceModel.rocks.setAdditionalVertexAttribute([this, vboForCapture] {
        this->glBindBuffer(GL_ARRAY_BUFFER, vboForCapture);
        this->glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), 0);
        this->glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        this->glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        this->glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
        this->glEnableVertexAttribArray(4);
        this->glEnableVertexAttribArray(5);
        this->glEnableVertexAttribArray(6);
        this->glEnableVertexAttribArray(7);
        this->glVertexAttribDivisor(4, 1);
        this->glVertexAttribDivisor(5, 1);
        this->glVertexAttribDivisor(6, 1);
        this->glVertexAttribDivisor(7, 1);
        });
}

void MyGLWindow::paintGL()
{
    mainCamera.caculateCamera();
    auto currentTime = std::chrono::steady_clock::now();
    auto passedDuration = duration_cast<duration<float>>(currentTime - lastTimePoint).count();
    auto timeFromBeginPoint = duration_cast<duration<float>>(currentTime - programBeginPoint).count();
    lastTimePoint = currentTime;

    //code here
    instanceModel.shader.bind();
    glUniformMatrix4fv(instanceModel.shader.uniformLocation("MV"), 1, GL_FALSE, value_ptr(mainCamera.viewProjectionMat()));
    instanceModel.planet.drawWithoutShaderBinding(&instanceModel.shader);
    instanceModel.rocks.instancedDrawWithoutShaderBinding(&instanceModel.shader, 100000);

    update();
}

void MyGLWindow::resizeGL(int w, int h)
{
    mainCamera.resizeCamera(w, h);
}

void MyGLWindow::mouseMoveEvent(QMouseEvent* event)
{
    float xAxisMove = event->x() - width() / 2;
    float yAxisMove = event->y() - height() / 2;
    QCursor myCursor = cursor();
    myCursor.setPos(mapToGlobal({ width() / 2, height() / 2 }));
    setCursor(myCursor);
    if (std::abs(xAxisMove) < 150.0f && std::abs(yAxisMove) < 150.0f)
    {
        mainCamera.processMouseMovement(xAxisMove, yAxisMove);
    }
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
