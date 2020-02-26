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
    resize(1200, 800);
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
    glGenFramebuffers(1, &frame.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, frame.fbo);
    glGenTextures(1, &frame.colorTex);
    glBindTexture(GL_TEXTURE_2D, frame.colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width(), height(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame.colorTex, 0);
    glGenTextures(1, &frame.depthTex);
    glBindTexture(GL_TEXTURE_2D, frame.depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width(), height(), 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, frame.depthTex, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        qInfo() << "frame buffer imcomplete";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

    //set up square
    glGenBuffers(1, &square.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, square.vbo);
    glBufferStorage(GL_ARRAY_BUFFER, square.vertices.size() * sizeof(float), square.vertices.data(), 0);
    glGenVertexArrays(1, &square.vao);
    glBindVertexArray(square.vao);
    glBindBuffer(GL_ARRAY_BUFFER, square.vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    const char* vertexShaderSource = R"(
#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inColor;

out vec3 Color;

void main()
{
    gl_Position = vec4(position, 1.0);
    Color = inColor;
}
)";

    square.shader.create();
    square.shader.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    square.shader.addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
#version 450 core
layout (location = 0) out vec4 Frag_Color;

in vec3 Color;

void main()
{
    Frag_Color = vec4(Color, 1.0);
}
)");
    square.shader.link();

    square.gammaCorrection.create();
    square.gammaCorrection.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    square.gammaCorrection.addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
#version 450 core
layout (location = 0) out vec4 Frag_Color;

in vec3 Color;

const float gammaCorrectionParameter = 1.0 / 2.2;

void main()
{
    vec3 result = pow(Color, vec3(gammaCorrectionParameter));
    Frag_Color = vec4(result, 1.0);
}
)");
    square.gammaCorrection.link();

    square.inverseGammaCorrection.create();
    square.inverseGammaCorrection.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    square.inverseGammaCorrection.addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
#version 450 core
layout (location = 0) out vec4 Frag_Color;

in vec3 Color;

const float gammaCorrectionParameter = 2.2;

void main()
{
    vec3 result = pow(Color, vec3(gammaCorrectionParameter));
    Frag_Color = vec4(result, 1.0);
}
)");
    square.inverseGammaCorrection.link();

    //set up box
    glPrimitiveRestartIndex(0xFFFF);
    glEnable(GL_PRIMITIVE_RESTART);
    glGenVertexArrays(1, &box.vao);
    glGenBuffers(1, &box.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, box.vbo);
    glBufferData(GL_ARRAY_BUFFER, box.vertices.size() * sizeof(float), box.vertices.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &box.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, box.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, box.indices.size() * sizeof(float), box.indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(box.vao);
    glBindBuffer(GL_ARRAY_BUFFER, box.vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, box.ebo);
    glBindVertexArray(0);

    //blinn-phong
    glGenVertexArrays(1, &bp.vao);
    glGenBuffers(1, &bp.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, bp.vbo);
    glBufferStorage(GL_ARRAY_BUFFER, bp.vertices.size() * sizeof(float), bp.vertices.data(), 0);
    glBindVertexArray(bp.vao);
    glBindBuffer(GL_ARRAY_BUFFER, bp.vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    QImage image("./images/wood_floor.jpg");
    image = image.convertToFormat(QImage::Format_RGBA8888).mirrored();
    glGenTextures(1, &bp.tex);
    glBindTexture(GL_TEXTURE_2D, bp.tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    bp.shader.create();
    bp.shader.addShaderFromSourceFile(QOpenGLShader::Vertex, "./shaders/blinnPhong.vert");
    bp.shader.addShaderFromSourceFile(QOpenGLShader::Fragment, "./shaders/blinnPhong.frag");
    bp.shader.link();

    bp.shader.bind();
    glUniform3f(bp.shader.uniformLocation("lightPos"), 0.0f, 0.5f, 0.0f);
}

void MyGLWindow::paintGL()
{
    mainCamera.caculateCamera();
    auto currentTime = std::chrono::steady_clock::now();
    auto passedDuration = duration_cast<duration<float>>(currentTime - lastTimePoint).count();
    auto timeFromBeginPoint = duration_cast<duration<float>>(currentTime - programBeginPoint).count();
    lastTimePoint = currentTime;

    //code here
    glBindFramebuffer(GL_FRAMEBUFFER, frame.fbo);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //blinn phong draw
    bp.shader.bind();
    glBindVertexArray(bp.vao);
    glUniformMatrix4fv(bp.shader.uniformLocation("VP"), 1, GL_FALSE, value_ptr(mainCamera.viewProjectionMat()));
    glUniform3fv(bp.shader.uniformLocation("viewPos"), 1, value_ptr(mainCamera.position));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bp.tex);
    glUniform1i(bp.shader.uniformLocation("floorTex"), 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //first
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    square.shader.bind();
    glBindVertexArray(square.vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, frame.fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFramebufferObject());
    glBlitFramebuffer(0, 0, width(), height(), 0, 0, width() / 3, height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

    //second
    glBindFramebuffer(GL_FRAMEBUFFER, frame.fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    square.gammaCorrection.bind();
    glBindVertexArray(square.vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, frame.fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFramebufferObject());
    glBlitFramebuffer(0, 0, width(), height(), width() / 3, 0, 2 * width() / 3, height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

    //third
    glBindFramebuffer(GL_FRAMEBUFFER, frame.fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    square.inverseGammaCorrection.bind();
    glBindVertexArray(square.vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, frame.fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFramebufferObject());
    glBlitFramebuffer(0, 0, width(), height(), 2 * width() / 3, 0, width(), height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

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
