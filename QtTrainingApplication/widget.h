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

    // �����뺯��
    void readBackground(QString path);
    void saveResult(QString path);
    void modelTransform(QString modelName, modelOperation op, modelDirection dir);
    void saveInfo(OpenGLSharedInfo& info);
    void updateFrameTime();
    QSize getSize();

    QVector<QOpenGLShaderProgram*>& getShaders();
    DirectionalLight* directionalLight; //ƽ�й�

    void setDemo(int v);
    int getDemo();

private:
    QOpenGLShaderProgram shaderProgram; //��Shader
    QOpenGLShaderProgram shadowShader;//��Ӱshader
    QOpenGLShaderProgram varianceShader;//VSM���˲���Shader
    void loadShader(QOpenGLShaderProgram& shaderProgram, QString vertPath, QString fragPath);
    //QOpenGLShaderProgram shadowShader;//��Ӱshader
    QVector<QOpenGLShaderProgram*> shaders;

    OpenGLSharedInfo& info; //��Ҫ�������ڴ��õ�����Ϣ
    BackgroundImage* back=NULL; //����

    QVector<Model*> models; //ģ���б�����
    QHash<QString, Model*> modelMap;    //ģ������ģ����ָ���map

    Camera camera;  //������࣬��������
    QTimer timer;   //��ʱ��������ͬ��

    void setUniformValuesShadow();
    void setUniformValuesModel();

    const int shadowWidth = 1920*4, shadowHeight = 1080*4;
    //QOpenGLShaderProgram shadowShaderProgram;
    QMatrix4x4 lightSpaceMatrix;

    //һ�����Ӱ��ͼ����
    GLuint frameBufferName = 0; //��Ӱ��framebuffer
    GLuint renderedTexture;
    GLuint depthrenderbuffer;   //��Ӱ��renderbuffer
    GLuint depthTexture;       //normal/VSM����
    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0};

    //VSM��Ӱ��ͼ����
    GLuint depthFBO;
    GLuint depthRBO;
    //GLuint depthTexture;  //����
    GLuint varianceFBO[2];
    GLuint varianceTexture[2];

    //���е���Ӱ��ͼ���յ���
    const int depthMapIndex = 15;
    //const int depthMap2Index = 13;

    //��¼֡����ʱ��
    QTimer* frameTimer;
    std::atomic<INT64> frameCount;  //������ֹ�̳߳�ͻ
    const int frameTimerInterval = 1000;//ms

    //��ͬ����Ӱ���ɷ�ʽ
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