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

    //在model中已经绑定好shader
    shader->bind();
    shader->setUniformValue("haveHeight", 0);    //默认不存在法线贴图
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glFunc->glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元
        // 获取纹理序号（diffuse_textureN 中的 N）
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
    // 绘制网格

    //如果没有纹理，将模型指定为白色
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

    // 名称 数据基础类型 起始地址 数据大小 数据间隔大小 
    // 为什么数据绑定需要shader参与？
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
