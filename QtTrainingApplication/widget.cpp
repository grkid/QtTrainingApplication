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
        back->draw();
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
    directionalLight->set(&shaderProgram);

    QMatrix4x4 lightProjection, lightView;
    lightProjection.ortho(-lightProjectionScale, lightProjectionScale, -lightProjectionScale, lightProjectionScale, nearPlane, farPlane);
    lightView.lookAt(directionalLight->getDirection() * lightDirectionIndex+modelCentreSum, QVector3D(0, 0, 0)+modelCentreSum, QVector3D(0, 1, 0));
    lightSpaceMatrix = lightProjection * lightView;
    shaderProgram.setUniformValue("lightSpaceMatrix", lightSpaceMatrix);
    shaderProgram.setUniformValue("depthMap", depthMapIndex);

    shaderProgram.setUniformValue("haveBackground", 0);
    if (back)
    {
        shaderProgram.setUniformValue("texture_background", back->textureIndex);
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

    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, shadowWidth, shadowHeight, 0, GL_RG, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthTexture, 0);
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
    glBindTexture(GL_TEXTURE_2D, depthTexture);

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

void Widget::updateFrameTime()
{
    INT64 frames = frameCount.load(std::memory_order_relaxed);
    double value = frames / (double)frameTimerInterval;
    //qDebug() << "frame time:" << value << " avg fps:" << frames;
    WOUT("frame time:" + QString::number(value) + " average fps:" + QString::number(frames));
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