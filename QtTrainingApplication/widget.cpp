#include "widget.h"
#include "MainWindow.h"

Widget::Widget(OpenGLSharedInfo& info,QWidget* parent)
    : QOpenGLWidget(parent)
    , camera(this)
    , info(info)
{

    timer.setInterval(18);
    connect(&timer, &QTimer::timeout, this, static_cast<void (Widget::*)()>(&Widget::update));
    timer.start();

    setMinimumSize(QSize(480, 360));
    
    // 抗锯齿
    QSurfaceFormat format;
    format.setSamples(16);
    setFormat(format);

    //帧生成时间
    frameTimer = new QTimer(this);
    frameTimer->setTimerType(Qt::PreciseTimer);
    connect(frameTimer, &QTimer::timeout, [=]() {updateFrameTime(); });
    frameTimer->start(frameTimerInterval);
}
Widget::~Widget()
{
    if(back)
        delete back;
    for (auto a : models) {
        delete a;
    }
    if (frameTimer)
        delete frameTimer;

    if (cubeMap)
        delete cubeMap;

    if (directionalLight)
        delete directionalLight;
}

void Widget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    loadShader(shaderProgram, "shader/model.vert", "shader/model.frag");
    loadShader(shadowShader, "shader/shadow.vert", "shader/shadow.frag");
    loadShader(varianceShader, "shader/variance.vert", "shader/variance.frag");

    shaders.append(&shaderProgram);
    shaders.append(&shadowShader);

    directionalLight = new DirectionalLight();


    for (int i=0,size=info.modelPaths.size();i<size;i++)
    {
        Model* newModel = Model::createModel(info.modelPaths[i], context());
        if (i<info.modelTransforms.size()) {
            newModel->setTransform(info.modelTransforms[i]);
        }
        models.append(newModel);
        modelMap.insert(newModel->getName(), newModel);
    }

    if (info.backgroundPath != "")
    {
        back = new BackgroundImage(info.backgroundPath, this);
        loadCubeTexture();
    }

    glEnable(GL_DEPTH_TEST);

    //WARNING: 开启BLEND会引起一系列问题！！！
    //包括但不限于无法渲染到RGB texture！！！
    //TODO
    /*glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

    camera.init();

    genFrameBufferVSM();
}

void Widget::paintGL()
{  
    //更新帧数
    frameCount++;

    //第一轮绘制，在引入了VSM后可能会有好几次
    glDisable(GL_BLEND);
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    shadowPassVSM();

    //第二轮绘制
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    setUniformValuesModel();
    if (back)
    {
        back->draw();
        shaderProgram.bind();
        shaderProgram.setUniformValue("texture_background_flat", back->textureIndex);
    }
    for (auto a : models) {
    a->draw(&shaderProgram);
    }
}

void Widget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

bool Widget::event(QEvent* e) {
    //camera.handle(e);
    return QWidget::event(e);
}

void Widget::setUniformValuesShadow()
{
    QVector3D modelCentreSum;
    for (auto item : models)
        modelCentreSum += item->getModelCentre();
    modelCentreSum /= models.size();

    QMatrix4x4 projection;
    projection.ortho(-lightProjectionScale, lightProjectionScale, -lightProjectionScale, lightProjectionScale, nearPlane, farPlane);
    QMatrix4x4 view;
    view.lookAt(directionalLight->getDirection() * lightDirectionIndex+modelCentreSum, QVector3D(0, 0, 0)+modelCentreSum, QVector3D(0, 1, 0));
    shadowShader.bind();
    shadowShader.setUniformValue("projection", projection);
    shadowShader.setUniformValue("view", view);
    shadowShader.setUniformValue("viewPos", camera.getCameraPos());
    shadowShader.setUniformValue("nearPlane", nearPlane);
    shadowShader.setUniformValue("farPlane", farPlane);
    //shaderProgram.setUniformValue("isFloor", 0);
    directionalLight->set(&shadowShader);
}

void Widget::setUniformValuesModel()
{
    QVector3D modelCentreSum;
    for (auto item : models)
        modelCentreSum += item->getModelCentre();
    modelCentreSum /= models.size();
    //qDebug() << "model centre:" << modelCentreSum;

    QMatrix4x4 projection;
    projection.perspective(45.0f, width() / (float)height(), 0.01, 500.0f);
    shaderProgram.bind();
    shaderProgram.setUniformValue("projection", projection);
    shaderProgram.setUniformValue("view", camera.getView());
    shaderProgram.setUniformValue("viewPos", camera.getCameraPos());
    shaderProgram.setUniformValue("nearPlane", nearPlane);
    shaderProgram.setUniformValue("farPlane", farPlane);
    shaderProgram.setUniformValue("haveDemo", haveDemo);
    shaderProgram.setUniformValue("haveFloorTransparent", haveFloorTransparent);
    directionalLight->set(&shaderProgram);

    QMatrix4x4 lightProjection, lightView;
    lightProjection.ortho(-lightProjectionScale, lightProjectionScale, -lightProjectionScale, lightProjectionScale, nearPlane, farPlane);
    lightView.lookAt(directionalLight->getDirection() * lightDirectionIndex+modelCentreSum, QVector3D(0, 0, 0)+modelCentreSum, QVector3D(0, 1, 0));
    lightSpaceMatrix = lightProjection * lightView;
    shaderProgram.setUniformValue("lightSpaceMatrix", lightSpaceMatrix);
    shaderProgram.setUniformValue("depthMap", depthMapIndex);

    shaderProgram.setUniformValue("haveBackground", 0);
    shaderProgram.setUniformValue("shadowIntensity", shadowIntensity);
    if (back)
    {
        cubeMap->bind(cubeTextureIndex);
        shaderProgram.setUniformValue("texture_background", cubeTextureIndex);
        //shaderProgram.setUniformValue("texture_background_flat", back->textureIndex);
        shaderProgram.setUniformValue("haveBackground", 1);
    }
}

void Widget::genFrameBufferNormal()
{
    //opengl tutorial 14
    glGenFramebuffers(1, &frameBufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferName);

    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, shadowWidth, shadowHeight, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

    glDrawBuffers(1, drawBuffers);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "GL_FRAMEBUFFER::not complete";
}

void Widget::shadowPassNormal()
{
    glViewport(0, 0, shadowWidth, shadowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferName);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setUniformValuesShadow();
    for (auto a : models) {
        a->draw(&shadowShader);
    }
    glActiveTexture(GL_TEXTURE0 + depthMapIndex);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
}

void Widget::genFrameBufferVSM()
{
    //https://github.com/Dilin71828/OpenGL_VSM/blob/master/src/OpenGL_VSM.cpp
    
    glGenFramebuffers(1, &depthFBO);
    glGenRenderbuffers(1, &depthRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, shadowWidth, shadowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);
    glEnable(GL_DEPTH_TEST);

    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };

    glGenTextures(1, &depthTextureVSM);
    glBindTexture(GL_TEXTURE_2D, depthTextureVSM);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, shadowWidth, shadowHeight, 0, GL_RG, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthTextureVSM, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(2, varianceFBO);
    glGenTextures(2, varianceTexture);

    for (int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, varianceFBO[i]);
        glBindTexture(GL_TEXTURE_2D, varianceTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, shadowWidth, shadowHeight, 0, GL_RG, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, varianceTexture[i], 0);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    //第二第三次渲染需要用
    float quadVertices[] = {
        // position        // uv
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void Widget::shadowPassVSM()
{
    //第一次渲染
    glViewport(0, 0, shadowWidth, shadowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setUniformValuesShadow();
    for (auto a : models) {
        a->draw(&shadowShader);
    }

    glActiveTexture(GL_TEXTURE0 + depthMapIndex);
    glBindTexture(GL_TEXTURE_2D, depthTextureVSM);

    //第二次渲染
    glBindFramebuffer(GL_FRAMEBUFFER, varianceFBO[0]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    varianceShader.bind();
    varianceShader.setUniformValue("horizontal", true);
    varianceShader.setUniformValue("depthTexture", depthMapIndex);
    renderQuadVSM();

    glActiveTexture(GL_TEXTURE0 + depthMapIndex);
    glBindTexture(GL_TEXTURE_2D, varianceTexture[0]);

    //第三次渲染
    glBindFramebuffer(GL_FRAMEBUFFER, varianceFBO[1]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    varianceShader.bind();
    varianceShader.setUniformValue("horizontal", false);
    varianceShader.setUniformValue("depthTexture", depthMapIndex);
    renderQuadVSM();

    glActiveTexture(GL_TEXTURE0 + depthMapIndex);
    glBindTexture(GL_TEXTURE_2D, varianceTexture[1]);
}

void Widget::renderQuadVSM()
{
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Widget::loadCubeTexture()
{
    /*
* 坐标系：
* Z轴垂直屏幕，方向朝外
* X轴水平，方向朝右
* Y轴垂直，方向朝上
*/
    QImage posx, posy, posz, negx, negy, negz;
    posx=QImage("image/skybox_img_left.jpg").mirrored(false,true).convertToFormat(QImage::Format_RGBA8888);
    negx = QImage("image/skybox_img_right.jpg").mirrored(false,true).convertToFormat(QImage::Format_RGBA8888);
    posy= QImage("image/skybox_img_down.jpg").mirrored(true,false).convertToFormat(QImage::Format_RGBA8888);
    negy= QImage("image/skybox_img_up.jpg").mirrored(true,false).convertToFormat(QImage::Format_RGBA8888);
    posz= QImage("image/skybox_img_front.jpg").mirrored(true,false).convertToFormat(QImage::Format_RGBA8888);
    negz= QImage("image/skybox_img_back.jpg").mirrored(false,true).convertToFormat(QImage::Format_RGBA8888);

    cubeMap = new QOpenGLTexture(QOpenGLTexture::TargetCubeMap);
    cubeMap->create();
    cubeMap->setSize(posx.width(), posx.height(), posx.depth());
    cubeMap->setFormat(QOpenGLTexture::RGBA8_UNorm);
    cubeMap->allocateStorage();

    cubeMap->setData(0, 0, QOpenGLTexture::CubeMapPositiveX,QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,(const void*)posx.constBits(), 0);
    cubeMap->setData(0, 0, QOpenGLTexture::CubeMapNegativeX, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, (const void*)negx.constBits(), 0);
    cubeMap->setData(0, 0, QOpenGLTexture::CubeMapPositiveY, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, (const void*)posy.constBits(), 0);
    cubeMap->setData(0, 0, QOpenGLTexture::CubeMapNegativeY, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, (const void*)negy.constBits(), 0);
    cubeMap->setData(0, 0, QOpenGLTexture::CubeMapPositiveZ, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, (const void*)posz.constBits(), 0);
    cubeMap->setData(0, 0, QOpenGLTexture::CubeMapNegativeZ, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, (const void*)negz.constBits(), 0);

    cubeMap->generateMipMaps();
    cubeMap->setWrapMode(QOpenGLTexture::ClampToEdge);
    cubeMap->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    cubeMap->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
}

void Widget::updateFrameTime()
{
    INT64 frames = frameCount.load(std::memory_order_relaxed);
    double seconds = frameTimerInterval / 1000.0;
    double value = frames / (double)frameTimerInterval;
    //qDebug() << "frame time:" << value << " avg fps:" << frames;
    WOUT("average fps in "+QString::number(seconds)+" seconds: " + QString::number(frames/seconds));
    frameCount.store(0, std::memory_order_relaxed);
}

QSize Widget::getSize()
{
    if (back)
        return back->getSize();
    return QSize(-1, -1);
}

QVector<QOpenGLShaderProgram*>& Widget::getShaders()
{
    return shaders;
}

void Widget::setDemo(int v)
{
    haveDemo = v;
}

int Widget::getDemo()
{
    return haveDemo;
}

void Widget::setFloorTransparent(int v)
{
    haveFloorTransparent = v;
}

int Widget::getFloorTransparent()
{
    return haveFloorTransparent;
}

void Widget::setShadowIntensity(float v)
{
    shadowIntensity = v;
}

float Widget::getShadowIntensity()
{
    return shadowIntensity;
}

void Widget::loadShader(QOpenGLShaderProgram& shader, QString vertPath, QString fragPath)
{
    if (!shader.addShaderFromSourceFile(QOpenGLShader::Vertex, vertPath)) {
        qDebug() << "ERROR:" << shader.log();
    }
    if (!shader.addShaderFromSourceFile(QOpenGLShader::Fragment, fragPath)) {
        qDebug() << "ERROR:" << shader.log();
    }
    if (!shader.link()) {
        qDebug() << "ERROR:" << shader.log();
    }
}

void Widget::readBackground(QString path)
{
    back->readBackground(path);
}

void Widget::saveResult(QString path)
{
    QSize initialSize = this->size();
    QSize size = back->getSize();
    if (size.isValid())
    {
        resize(size);
        QImage image = grabFramebuffer();
        image.save(path);
    }
    resize(initialSize);
}

void Widget::modelTransform(QString modelName, modelOperation op, modelDirection dir)
{
    //TODO:修改模型名为真正的模型名
    Model* model = modelMap.value(modelName);
    if (!model)
    {
        //qDebug() << "can't find model for model name: " << modelName;
        WOUT("can't find model for model name: "+modelName);
        return;
    }

    model->modelTransform(op, dir);
}

void Widget::saveInfo(OpenGLSharedInfo& info)
{
    info.modelTransforms.clear();
    for (auto item : models)
    {
        info.modelTransforms.append(item->getTransform());
    }
    if (info.modelPaths.size() != info.modelTransforms.size())
    {
        //qDebug() << "WIDGET::info size incorrect.";
        WOUT("WIDGET::info size incorrect.");
    }
}