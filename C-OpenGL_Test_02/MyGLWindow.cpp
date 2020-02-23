#include "MyGLWindow.h"

#include<qimage.h>
#include<QKeyEvent>
#include<cmath>

using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using glm::mat4;
using glm::mat3;
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

    glGenTextures(1, &fboTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 800, 0, GL_RGBA, GL_UNSIGNED_INT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTex, 0);

    glGenTextures(1, &fboDepthTex);
    glBindTexture(GL_TEXTURE_2D, fboDepthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 800, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fboDepthTex, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
        qInfo() << "frame buffer init success.";
    }
    else
    {
        qInfo() << "frame buffer init failed.";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

    fboShader.create();
    fboShader.addShaderFromSourceFile(QOpenGLShader::Vertex, "./shaders/fboShader.vert");
    fboShader.addShaderFromSourceFile(QOpenGLShader::Fragment, "./shaders/fboShader.frag");
    fboShader.link();

    fboBox.init();
    fboBox.resetRotateMat(rotate(mat4{ 1.0f }, radians(55.0f), vec3(0.5f, 1.0f, 0.0f)));

    glGenVertexArrays(1, &post.vao);
    glGenBuffers(1, &post.vbo);
    glGenBuffers(1, &post.ebo);
    glBindBuffer(GL_ARRAY_BUFFER, post.vbo);
    glBufferData(GL_ARRAY_BUFFER, screenVertices.size() * sizeof(float), screenVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, post.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, screenIndices.size() * sizeof(unsigned int), screenIndices.data(), GL_STATIC_DRAW);
    glBindVertexArray(post.vao);
    glBindBuffer(GL_ARRAY_BUFFER, post.vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, post.ebo);
    glBindVertexArray(0);

    glGenFramebuffers(1, &post.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, post.fbo);
    glGenTextures(1, &post.tex);
    glBindTexture(GL_TEXTURE_2D, post.tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width(), height(), 0, GL_RGBA, GL_UNSIGNED_INT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, post.tex, 0);
    glGenTextures(1, &post.depthTex);
    glBindTexture(GL_TEXTURE_2D, post.depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width(), height(), 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, post.depthTex, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        qInfo() << "post process frame buffer create failed.";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

    post.shader.create();
    post.shader.addShaderFromSourceFile(QOpenGLShader::Vertex, "./shaders/postProcess.vert");
    post.shader.addShaderFromSourceFile(QOpenGLShader::Fragment, "./shaders/postProcess.frag");
    post.shader.link();

    {
        glGenTextures(1, &cubeMap.tex);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.tex);
        std::string filePath = "./images/skybox/";
        std::string suffix = ".jpg";
        int i = 0;
        for (auto const& fileName : { "right","left","top","bottom","front","back" })
        {
            std::string file = filePath + fileName + suffix;
            QImage image(QString::fromStdString(filePath + fileName + suffix));
            image.convertTo(QImage::Format_ARGB32);
            const unsigned char* data = image.bits();
            if (!data)
            {
                qInfo() << "cube map image invalid.\n";
            }
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i++, 0, GL_RGB, image.width(), image.height(), 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, image.bits());
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    glGenVertexArrays(1, &cubeMap.vao);
    glGenBuffers(1, &cubeMap.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, cubeMap.vbo);
    glBufferData(GL_ARRAY_BUFFER, CubeMap::vertices.size() * sizeof(float), CubeMap::vertices.data(), GL_STATIC_DRAW);
    glBindVertexArray(cubeMap.vao);
    glBindBuffer(GL_ARRAY_BUFFER, cubeMap.vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    cubeMap.shader.create();
    cubeMap.shader.addShaderFromSourceFile(QOpenGLShader::Vertex, "./shaders/cubeMapShader.vert");
    cubeMap.shader.addShaderFromSourceFile(QOpenGLShader::Fragment, "./shaders/cubeMapShader.frag");
    cubeMap.shader.link();

    cubeReflectShader.create();
    cubeReflectShader.addShaderFromSourceFile(QOpenGLShader::Vertex, "./shaders/environmentMapping.vert");
    cubeReflectShader.addShaderFromSourceFile(QOpenGLShader::Fragment, "./shaders/environmentMapping.frag");
    cubeReflectShader.link();
}

void MyGLWindow::paintGL()
{
    mainCamera.caculateCamera();
    auto currentTime = std::chrono::steady_clock::now();
    auto passedDuration = duration_cast<duration<float>>(currentTime - lastTimePoint).count();
    auto timeFromBeginPoint = duration_cast<duration<float>>(currentTime - programBeginPoint).count();
    lastTimePoint = currentTime;

    //inner camera
    vec3 innerPosition;
    innerPosition.x = 2.0f * sin(timeFromBeginPoint);
    innerPosition.z = 2.0f * cos(timeFromBeginPoint);
    mat4 innerView = lookAt(innerPosition, vec3(), vec3(0.0f, 1.0f, 0.0f));
    mat4 innerProjection = perspective(45.0f, 800.0f / 800.0f, 0.1f, 10.0f);
    mat4 innerVP = innerProjection * innerView;

    //bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glViewport(0, 0, 800, 800);

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
        glUniformMatrix4fv(lightBoxShader.uniformLocation("MVP"), 1, GL_FALSE, value_ptr(innerVP * lightModel));
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
    glUniformMatrix4fv(modelShader.uniformLocation("MVP"), 1, GL_FALSE, value_ptr(innerVP * modelMat));
    glUniformMatrix4fv(modelShader.uniformLocation("modelMat"), 1, GL_FALSE, value_ptr(modelMat));
    glUniform3fv(modelShader.uniformLocation("viewPos"), 1, value_ptr(innerPosition));
    testModel.draw(&modelShader);

    //draw to post process tex
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

    glViewport(0, 0, width(), height());
    glClearColor(0.27f, 0.27f, 0.27f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    fboBox.rotateMat = rotate(fboBox.rotateMat, radians(passedDuration * 10.0f), vec3(0.0f, 0.5f, 1.0f));
    cubeReflectShader.bind();
    fboBox.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.tex);
    glUniform1i(cubeReflectShader.uniformLocation("cubeMap"), 0);
    glUniform3fv(cubeReflectShader.uniformLocation("viewPos"), 1, value_ptr(mainCamera.position));
    glUniformMatrix4fv(cubeReflectShader.uniformLocation("modelMat"), 1, GL_FALSE, value_ptr(fboBox.getModelMat()));
    glUniformMatrix4fv(cubeReflectShader.uniformLocation("MVP"), 1, GL_FALSE, value_ptr(mainCamera.viewProjectionMat() * fboBox.getModelMat()));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboTex);
    glUniform1i(fboShader.uniformLocation("tex"), 0);
    fboBox.draw();

    //cube map
    cubeMap.shader.bind();
    glBindVertexArray(cubeMap.vao);
    mat4 VPMap = mainCamera.projectionMat * lookAt(vec3(0.0f), mainCamera.front, vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(cubeMap.shader.uniformLocation("VP"), 1, GL_FALSE, value_ptr(VPMap));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.tex);
    glUniform1i(cubeMap.shader.uniformLocation("tex"), 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);

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
    if (std::abs(xAxisMove) > 150.0f || std::abs(yAxisMove) > 150.0f)
    {
        return;
    }
    mainCamera.processMouseMovement(xAxisMove, yAxisMove);
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
