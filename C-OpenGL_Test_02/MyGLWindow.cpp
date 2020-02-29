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
using glm::ortho;

MyGLWindow::MyGLWindow(QWidget* parent)
    : QOpenGLWidget(parent)
{
    resize(1920, 1080);
    setCursor(Qt::BlankCursor);
    setWindowFlag(Qt::FramelessWindowHint);
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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    //set up box
    glPrimitiveRestartIndex(0xFFFF);
    glEnable(GL_PRIMITIVE_RESTART);
    glGenVertexArrays(1, &box.vao);
    glGenBuffers(1, &box.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, box.vbo);
    glBufferData(GL_ARRAY_BUFFER, box.vertices.size() * sizeof(float), box.vertices.data(), GL_STATIC_DRAW);
    glBindVertexArray(box.vao);
    glBindBuffer(GL_ARRAY_BUFFER, box.vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    //init box model mat
    std::vector<mat4> modelMats;
    modelMats.push_back(mat4{ 1.0f });
    modelMats.push_back(translate(mat4{ 1.0f }, vec3{ 0.0f,1.5f,0.0f }) * scale(mat4{ 1.0f }, vec3(0.5f)));
    modelMats.push_back(translate(mat4{ 1.0f }, vec3(-1.0f, 0.0f, 2.0f))
        * rotate(mat4{ 1.0f }, radians(60.0f), normalize(vec3(1.0f, 0.0f, 1.0f)))
        * scale(mat4{ 1.0f }, vec3(0.25f)));
    glGenBuffers(1, &box.modelMatVbo);
    glBindBuffer(GL_ARRAY_BUFFER, box.modelMatVbo);
    glBufferStorage(GL_ARRAY_BUFFER, modelMats.size() * sizeof(mat4), modelMats.data(), 0);
    glBindVertexArray(box.vao);
    glBindBuffer(GL_ARRAY_BUFFER, box.modelMatVbo);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), 0);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(4 * sizeof(float)));
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(8 * sizeof(float)));
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(12 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    glEnableVertexAttribArray(6);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glBindVertexArray(0);

    //init plane
    glGenVertexArrays(1, &plane.vao);
    glGenBuffers(1, &plane.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, plane.vbo);
    glBufferStorage(GL_ARRAY_BUFFER, plane.planeVertices.size() * sizeof(float), plane.planeVertices.data(), 0);
    glBindVertexArray(plane.vao);
    glBindBuffer(GL_ARRAY_BUFFER, plane.vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    mat4 planeModel = mat4{ 1.0f };
    glVertexAttrib4fv(3, value_ptr(planeModel[0]));
    glVertexAttrib4fv(4, value_ptr(planeModel[1]));
    glVertexAttrib4fv(5, value_ptr(planeModel[2]));
    glVertexAttrib4fv(6, value_ptr(planeModel[3]));
    glBindVertexArray(0);

    //init plane tex
    QImage image = QImage("./images/texture_test.jpg").convertToFormat(QImage::Format_RGBA8888).mirrored();
    glGenTextures(1, &plane.tex);
    glBindTexture(GL_TEXTURE_2D, plane.tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //init depth map fbo
    glGenFramebuffers(1, &ldMap.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, ldMap.fbo);
    glGenTextures(1, &ldMap.depthMap);
    glBindTexture(GL_TEXTURE_2D, ldMap.depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f,1.0f,1.0f,1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ldMap.depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

    //init test shader
    testShader.create();
    testShader.addShaderFromSourceFile(QOpenGLShader::Vertex, "./shaders/normalMapping.vert");
    testShader.addShaderFromSourceFile(QOpenGLShader::Fragment, "./shaders/normalMapping.frag");
    testShader.link();

    //init light map shader
    lightMapShader.create();
    lightMapShader.addShaderFromSourceFile(QOpenGLShader::Vertex, "./shaders/lightMapping.vert");
    lightMapShader.addShaderFromSourceFile(QOpenGLShader::Fragment, "./shaders/emptyFrag.frag");
    lightMapShader.link();

    //init normal map
    QImage normalImage = QImage("./images/normal_map.png").convertToFormat(QImage::Format_RGBA8888).mirrored();
    glGenTextures(1, &normalTex);
    glBindTexture(GL_TEXTURE_2D, normalTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, normalImage.width(), normalImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, normalImage.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    auto caculateTB = [](std::array<vec3,3> vertices,std::array<vec2,3> texCoords)
        ->std::array<vec3, 2>
    {
        vec3 e1 = vertices[1] - vertices[0];
        vec3 e2 = vertices[2] - vertices[0];
        float u1 = texCoords[1].x - texCoords[0].x;
        float u2 = texCoords[2].x - texCoords[0].x;
        float v1 = texCoords[1].y - texCoords[0].y;
        float v2 = texCoords[2].y - texCoords[0].y;
        glm::mat3x2 edgeMat;
        edgeMat[0] = vec2(e1.x, e2.x);
        edgeMat[1] = vec2(e1.y, e2.y);
        edgeMat[2] = vec2(e1.z, e2.z);

        glm::mat2 deltaMat;
        deltaMat[0] = vec2(u1, u2);
        deltaMat[1] = vec2(v1, v2);

        glm::mat3x2 tangentMat = glm::inverse(deltaMat) * edgeMat;
        vec3 t(tangentMat[0].x, tangentMat[1].x, tangentMat[2].x);
        vec3 b(tangentMat[0].y, tangentMat[1].y, tangentMat[2].y);
        return std::array<vec3, 2>{t, b};
    };

    //caculate btn for box
    {
        std::vector<vec3> vertices;
        std::vector<vec2> texCoords;
        for (auto i = box.vertices.begin(); i != box.vertices.end(); i += 8)
        {
            vertices.push_back(vec3(*i, *(i + 1), *(i + 2)));
            texCoords.push_back(vec2(*(i + 6), *(i + 7)));
        }

        std::vector<vec3> tangentSpace;
        for (auto i = 0; i < vertices.size(); i += 3)
        {
            std::array<vec3, 3> triangleVert{ vertices[i],vertices[i + 1],vertices[i + 2] };
            std::array<vec2, 3> tiangleTexCoords{ texCoords[i],texCoords[i + 1],texCoords[i + 2] };
            auto TB = caculateTB(triangleVert, tiangleTexCoords);
            tangentSpace.push_back(TB[0]);
            tangentSpace.push_back(TB[1]);
            tangentSpace.push_back(TB[0]);
            tangentSpace.push_back(TB[1]);
            tangentSpace.push_back(TB[0]);
            tangentSpace.push_back(TB[1]);
        }

        glGenBuffers(1, &box.tbnBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, box.tbnBuffer);
        glBufferData(GL_ARRAY_BUFFER, tangentSpace.size() * sizeof(vec3), tangentSpace.data(), GL_STATIC_DRAW);

        glBindVertexArray(box.vao);
        glBindBuffer(GL_ARRAY_BUFFER, box.tbnBuffer);
        glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
        glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(7);
        glEnableVertexAttribArray(8);
    }

    //caculate btn for plane
    {
        std::vector<vec3> vertices;
        std::vector<vec2> texCoords;
        for (auto i = plane.planeVertices.begin(); i != plane.planeVertices.end(); i += 8)
        {
            vertices.push_back(vec3(*i, *(i + 1), *(i + 2)));
            texCoords.push_back(vec2(*(i + 6), *(i + 7)));
        }

        std::vector<vec3> tangentSpace;
        for (auto i = 0; i < vertices.size(); i += 3)
        {
            std::array<vec3, 3> triangleVert{ vertices[i],vertices[i + 1],vertices[i + 2] };
            std::array<vec2, 3> tiangleTexCoords{ texCoords[i],texCoords[i + 1],texCoords[i + 2] };
            auto TB = caculateTB(triangleVert, tiangleTexCoords);
            tangentSpace.push_back(TB[0]);
            tangentSpace.push_back(TB[1]);
            tangentSpace.push_back(TB[0]);
            tangentSpace.push_back(TB[1]);
            tangentSpace.push_back(TB[0]);
            tangentSpace.push_back(TB[1]);
        }

        glGenBuffers(1, &plane.tbnBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, plane.tbnBuffer);
        glBufferData(GL_ARRAY_BUFFER, tangentSpace.size() * sizeof(vec3), tangentSpace.data(), GL_STATIC_DRAW);

        glBindVertexArray(plane.vao);
        glBindBuffer(GL_ARRAY_BUFFER, plane.tbnBuffer);
        glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
        glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(7);
        glEnableVertexAttribArray(8);
    }
}

void MyGLWindow::paintGL()
{
    mainCamera.caculateCamera();
    auto currentTime = std::chrono::steady_clock::now();
    float passedDuration = duration_cast<duration<float>>(currentTime - lastTimePoint).count();
    float timeFromBeginPoint = duration_cast<duration<float>>(currentTime - programBeginPoint).count();
    lastTimePoint = currentTime;

    auto drawScene = [this] {
        glBindVertexArray(box.vao);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 3);
        glBindVertexArray(plane.vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    };

    //draw from light position
    glBindFramebuffer(GL_FRAMEBUFFER, ldMap.fbo);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
    lightMapShader.bind();
    glViewport(0, 0, 1024, 1024);
    float lightNear = 1.0f, lightFar = 7.0f;
    mat4 lightOrtho = ortho(-10.0f, 10.0f, -10.0f, 10.0f, lightNear, lightFar);
    mat4 lightView = lookAt(vec3(-2.0f, 4.0f, -1.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 lightVO = lightOrtho * lightView;
    glUniformMatrix4fv(lightMapShader.uniformLocation("lightVP"), 1, GL_FALSE, value_ptr(lightVO));
    drawScene();
    glViewport(0, 0, width(), height());
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glCullFace(GL_BACK);

    //draw scene
    testShader.bind();
    glUniformMatrix4fv(testShader.uniformLocation("VP"), 1, GL_FALSE, value_ptr(mainCamera.viewProjectionMat()));
    glUniformMatrix4fv(testShader.uniformLocation("lightSpaceVO"), 1, GL_FALSE, value_ptr(lightVO));
    glUniform3fv(testShader.uniformLocation("lightPos"), 1, value_ptr(vec3(-2.0f, 4.0f, -1.0f)));
    glUniform3fv(testShader.uniformLocation("viewPos"), 1, value_ptr(mainCamera.position));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, plane.tex);
    glUniform1i(testShader.uniformLocation("tex"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ldMap.depthMap);
    glUniform1i(testShader.uniformLocation("shadowMap"), 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, normalTex);
    glUniform1i(testShader.uniformLocation("normalMap"), 2);
    drawScene();

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
