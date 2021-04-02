#include "Shadow.h"

void Shadow::draw()
{
    shaderProgram->bind();
    shaderProgram->setUniformValue("transform", QMatrix4x4());
    shaderProgram->setUniformValue("model", QMatrix4x4());

    shaderProgram->setAttributeBuffer("aPos", GL_FLOAT, 0, 3, sizeof(Vertex));
    shaderProgram->enableAttributeArray("aPos");

    shaderProgram->setAttributeBuffer("aNormal", GL_FLOAT, offsetof(Vertex, Normal), 3, sizeof(Vertex));
    shaderProgram->enableAttributeArray("aNormal");

    shaderProgram->setAttributeBuffer("aTexCoords", GL_FLOAT, offsetof(Vertex, TexCoords), 2, sizeof(Vertex));
    shaderProgram->enableAttributeArray("aTexCoords");

    const unsigned int shadowTextureIndex = 1;

    texture.bind(shadowTextureIndex);
    shaderProgram->setUniformValue(" texture_diffuse1", 0);
    shaderProgram->setUniformValue(" texture_specular1", 2);
    shaderProgram->setUniformValue(" texture_ambient1", 1);
    shaderProgram->setUniformValue(" texture_height1", shadowTextureIndex);

    QOpenGLVertexArrayObject::Binder{ &VAO };
    widget->context()->functions()->glDrawArrays(GL_POLYGON, 0, 4);
    VAO.release();
}



Shadow::Shadow(QOpenGLWidget* widget, QOpenGLShaderProgram* shader)
    : VBO(QOpenGLBuffer::VertexBuffer)
    , texture(QOpenGLTexture::Target2D)
{
    vertices = {
        // λ��           //��������                 //��������
         1.0f, 1.0f, 0.999f,   0.0,0.0,0.0,    1.0f, 0.0f,  0,0,0,0,0,0, // ����
        1.0f, -1.0f, 0.999f,   0.0,0.0,0.0,     0.0f, 0.0f,  0,0,0,0,0,0,  // ����
        -1.0f,  -1.0f, 0.999f,  0.0,0.0,0.0,    0.0f, 1.0f,  0,0,0,0,0,0,  // ����
         -1.0f,  1.0f, 0.999f,    0.0,0.0,0.0,     1.0f, 1.0f, 0,0,0,0,0,0,   // ����
    };

    this->widget = widget;
    this->shaderProgram = shader;

    //VAO,VBO���
    VAO.create();
    VAO.bind();

    VBO.create();       //����VBO����
    VBO.bind();         //��VBO�󶨵���ǰ�Ķ��㻺�����QOpenGLBuffer::VertexBuffer����

    //���������ݷ��䵽VBO�У���һ������Ϊ����ָ�룬�ڶ�������Ϊ���ݵ��ֽڳ���
    VBO.allocate(vertices.data(), sizeof(float) * vertices.size());
    texture.create();
    image = QImage("image/container.jpg");
    texture.setData(image.mirrored());
    texture.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
    texture.setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
    texture.setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);

    VAO.release();
}
