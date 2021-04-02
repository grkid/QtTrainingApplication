#include "mesh.h"
#include <QtOpenGLExtensions/QOpenGLExtensions>
#include "MainWindow.h"

Mesh::Mesh(QOpenGLFunctions* glFunc, aiMatrix4x4 model)
    : glFunc(glFunc)
    , VBO(QOpenGLBuffer::VertexBuffer)
    , EBO(QOpenGLBuffer::IndexBuffer)
{

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            this->model(i, j) = model[i][j];
        }
    }
}
void Mesh::draw(QOpenGLShaderProgram* shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int ambientNr = 1;
    unsigned int heightNr = 1;

    //��model���Ѿ��󶨺�shader
    shader->bind();
    shader->setUniformValue("haveHeight", 0);    //Ĭ�ϲ����ڷ�����ͼ
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glFunc->glActiveTexture(GL_TEXTURE0 + i); // �ڰ�֮ǰ������Ӧ������Ԫ
        // ��ȡ������ţ�diffuse_textureN �е� N��
        QString number;
        QString name = textures[i]->type;
        if (name == "texture_diffuse")
            number = QString::number(diffuseNr++);
        else if (name == "texture_specular")
            number = QString::number(specularNr++);
        else if (name == "texture_ambient")
            number = QString::number(ambientNr++); // transfer unsigned int to stream
        else if (name == "texture_height")
        {
            shader->setUniformValue("haveHeight", 1);
            number = QString::number(heightNr++); // transfer unsigned int to stream
        }
        textures[i]->texture.bind();
        shader->setUniformValue((name + number).toStdString().c_str(), i);
    }
    // ��������

    //���û��������ģ��ָ��Ϊ��ɫ
    if (textures.size() == 0) {
        shader->setUniformValue("haveTexture", 0);
    } else {
        shader->setUniformValue("haveTexture", 1);
    }

    //QOpenGLVertexArrayObject::Binder bind(&VAO);
    VAO.bind();
    //setAttributeArray(shader);
    shader->setUniformValue("model", model);

    glFunc->glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
void Mesh::setupMesh()
{

    VAO.create();
    VAO.bind();

    VBO.create();
    EBO.create();

    VBO.bind();
    VBO.allocate(vertices.data(), vertices.size() * sizeof(Vertex));

    EBO.bind();
    EBO.allocate(indices.data(), indices.size() * sizeof(unsigned int));

    // ���� ���ݻ������� ��ʼ��ַ ���ݴ�С ���ݼ����С 
    // Ϊʲô���ݰ���Ҫshader���룿
    auto shaders = MainWindow::getOpenGLWidget()->getShaders();
    for (auto shaderProgram : shaders)
    {
        shaderProgram->setAttributeBuffer("aPos", GL_FLOAT, 0, 3, sizeof(Vertex));
        shaderProgram->enableAttributeArray("aPos");

        shaderProgram->setAttributeBuffer("aNormal", GL_FLOAT, offsetof(Vertex, Normal), 3, sizeof(Vertex));
        shaderProgram->enableAttributeArray("aNormal");

        shaderProgram->setAttributeBuffer("aTexCoords", GL_FLOAT, offsetof(Vertex, TexCoords), 2, sizeof(Vertex));
        shaderProgram->enableAttributeArray("aTexCoords");

        shaderProgram->setAttributeBuffer("aTangent", GL_FLOAT, offsetof(Vertex, Tangent), 3, sizeof(Vertex));
        shaderProgram->enableAttributeArray("aTangent");

        shaderProgram->setAttributeBuffer("aBitangent", GL_FLOAT, offsetof(Vertex, Bitangent), 3, sizeof(Vertex));
        shaderProgram->enableAttributeArray("aBitangent");
    }

    VAO.release();
}
