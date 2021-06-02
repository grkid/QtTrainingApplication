#include "backgroundImage.h"

void BackgroundImage::draw()
{
    //widget->context()->functions()->glClearColor(0.1f, 0.5f, 0.7f, 1.0f);  //����������ɫ
    //widget->context()->functions()->glClear(GL_COLOR_BUFFER_BIT);       //�����ɫ����
    shaderProgram.bind();                     //ʹ��shaderProgram��ɫ����
    
    const int textureIndex = 14;
    texture.bind(textureIndex);                                    //��texture�󶨵�����Ԫ0

    //debug���룬���ڲ鿴���ͼ
    shaderProgram.setUniformValue("Texture", 14);      //����ɫ������ourTexture������Ԫ0�л�ȡ��������
    QOpenGLVertexArrayObject::Binder{ &VAO };
    widget->context()->functions()->glDrawArrays(GL_POLYGON, 0, 4);     //ʹ����0��ʼ������Ϊ4�Ķ������������ƶ����
    //VAO.release();
    
}

BackgroundImage::BackgroundImage(QString path, QOpenGLWidget* widget)
    : VBO(QOpenGLBuffer::VertexBuffer)
    , texture(QOpenGLTexture::Target2D)
{
    const float _zmax=0.9999999f;
    vertices = {
        // λ��                            //��������
         1.0f, -1.0f, _zmax,    1.0f, 0.0f,   // ����
        -1.0f, -1.0f, _zmax,     0.0f, 0.0f,   // ����
        -1.0f,  1.0f, _zmax,    0.0f, 1.0f,   // ����
         1.0f,  1.0f, _zmax,     1.0f, 1.0f,   // ����
    };

    this->widget = widget;

    if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "shader/background.vert")) {     //��Ӳ����붥����ɫ��
        qDebug() << "ERROR:" << shaderProgram.log();    //����������,��ӡ������Ϣ
    }
    if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "shader/background.frag")) {   //��Ӳ�����Ƭ����ɫ��
        qDebug() << "ERROR:" << shaderProgram.log();    //����������,��ӡ������Ϣ
    }
    if (!shaderProgram.link()) {                      //������ɫ��
        qDebug() << "ERROR:" << shaderProgram.log();    //������ӳ���,��ӡ������Ϣ
    }

    QOpenGLVertexArrayObject::Binder{ &VAO };

    VBO.create();       //����VBO����
    VBO.bind();         //��VBO�󶨵���ǰ�Ķ��㻺�����QOpenGLBuffer::VertexBuffer����

    //���������ݷ��䵽VBO�У���һ������Ϊ����ָ�룬�ڶ�������Ϊ���ݵ��ֽڳ���
    VBO.allocate(vertices.data(), sizeof(float) * vertices.size());
    texture.create();
    image = QImage(path);
    texture.setData(image.mirrored());
    texture.setAutoMipMapGenerationEnabled(FALSE);//��ֹ����ģ��
    //texture.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
    texture.setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
    texture.setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);

    //���ö��������ʽ�������ö���
    shaderProgram.setAttributeBuffer("aPos", GL_FLOAT, 0, 3, sizeof(GLfloat) * 5);
    shaderProgram.enableAttributeArray("aPos");
    shaderProgram.setAttributeBuffer("aTexCoord", GL_FLOAT, sizeof(GLfloat) * 3, 2, sizeof(GLfloat) * 5);
    shaderProgram.enableAttributeArray("aTexCoord");
}

BackgroundImage::~BackgroundImage()
{
    //��ʱû����Ҫ�����
}

QSize BackgroundImage::getSize()
{
    return image.size();
}

void BackgroundImage::readBackground(QString path)
{
    image = QImage(path);
    texture.setData(image.mirrored());
}


