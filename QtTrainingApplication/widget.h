#ifndef WIDGET_H
#define WIDGET_H

//Qt
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>
#include <QWidget>
#include <QSize>
#include <QPixmap>
#include <QHash>
#include <QOpenGLFrameBufferObject>
#include <QVector>

//STL
#include <atomic>

//project
#include "backgroundImage.h"
#include "OpenGLSharedInfo.h"
#include "model.h"
#include "DirectionalLight.h"
//#include "Shadow.h"
#include "mesh.h"
#include "camera.h"

class Widget : public QOpenGLWidget, public QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    Widget(OpenGLSharedInfo& info,QWidget* parent = 0);
    ~Widget();
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual bool event(QEvent* e) override;

    // 可重入函数
    void readBackground(QString path);
    void saveResult(QString path);
    void modelTransform(QString modelName, modelOperation op, modelDirection dir);
    void saveInfo(OpenGLSharedInfo& info);
    void updateFrameTime();
    QSize getSize();

    QVector<QOpenGLShaderProgram*>& getShaders();
    DirectionalLight* directionalLight; //平行光

    void setDemo(int v);
    int getDemo();

private:
    QOpenGLShaderProgram shaderProgram; //主Shader
    QOpenGLShaderProgram shadowShader;//阴影shader
    QOpenGLShaderProgram varianceShader;//VSM的滤波器Shader
    void loadShader(QOpenGLShaderProgram& shaderProgram, QString vertPath, QString fragPath);
    //QOpenGLShaderProgram shadowShader;//阴影shader
    QVector<QOpenGLShaderProgram*> shaders;

    OpenGLSharedInfo& info; //需要从主窗口处拿到的信息
    BackgroundImage* back=NULL; //背景

    QVector<Model*> models; //模型列表，多余
    QHash<QString, Model*> modelMap;    //模型名，模型类指针的map

    Camera camera;  //摄像机类，基本不动
    QTimer timer;   //计时器，用于同步

    void setUniformValuesShadow();
    void setUniformValuesModel();

    const int shadowWidth = 1920*4, shadowHeight = 1080*4;
    //QOpenGLShaderProgram shadowShaderProgram;
    QMatrix4x4 lightSpaceMatrix;

    //一般的阴影贴图所需
    GLuint frameBufferName = 0; //阴影用framebuffer
    GLuint renderedTexture;
    GLuint depthrenderbuffer;   //阴影用renderbuffer
    GLuint depthTexture;       //normal/VSM公用
    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0};

    //VSM阴影贴图所需
    GLuint depthFBO;
    GLuint depthRBO;
    //GLuint depthTexture;  //已有
    GLuint varianceFBO[2];
    GLuint varianceTexture[2];

    //所有的阴影贴图最终导向
    const int depthMapIndex = 15;
    //const int depthMap2Index = 13;

    //记录帧生成时间
    QTimer* frameTimer;
    std::atomic<INT64> frameCount;  //加锁防止线程冲突
    const int frameTimerInterval = 1000;//ms

    //不同的阴影生成方式
    void genFrameBufferNormal();
    void shadowPassNormal();

    void genFrameBufferVSM();
    void shadowPassVSM();
    GLuint quadVAO;
    GLuint quadVBO;
    void renderQuadVSM();

    const float nearPlane = 0.1;
    const float farPlane = 30.0;
    const int lightDirectionIndex = 5;
    const float lightProjectionScale = 20.0;

    int haveDemo = 0;
};

#endif // WIDGET_H