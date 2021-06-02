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

/*
贴图适配原则：
至少会有一张漫反射/环境光图，没有的话这两者混用
如果没有镜面反射图，反射率等于specular强度，否则反射率等于镜面反射图
*/
void Mesh::draw(QOpenGLShaderProgram* shader)
{
    /*unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int ambientNr = 1;
    unsigned int heightNr = 1;*/

    int diffuseIndex=-1, specularIndex=-1, ambientIndex=-1, heightIndex=-1;

    int haveDiffuse = 0, haveSpecular = 0, haveAmbient = 0, haveHeight = 0;

    //在model中已经绑定好shader
    shader->bind();
    shader->setUniformValue("haveHeight", 0);    //默认不存在法线贴图
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glFunc->glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元
        //index与shader中index对应
        // 获取纹理序号（diffuse_textureN 中的 N）
        QString number = "1";
        QString name = textures[i]->type;
        if (name == "texture_diffuse") {
            haveDiffuse = 1;
            diffuseIndex = i;
        }
        else if (name == "texture_specular") {
            haveSpecular = 1;
            shader->setUniformValue("haveSpecular", haveSpecular);
            specularIndex = i;
        }
        else if (name == "texture_ambient") {
            haveAmbient = 1;
            ambientIndex = i;
        }
        else if (name == "texture_height")
        {
            haveHeight = 1;
            shader->setUniformValue("haveHeight", haveHeight);
            heightIndex = i;
        }
        textures[i]->texture.bind(i);
        std::string stdstr = (name + number).toLocal8Bit().data();
        shader->setUniformValue(stdstr.c_str(), i);
    }

    //贴图适配
    if (haveDiffuse || haveAmbient)
    {
        if (!haveDiffuse) 
        {
            glFunc->glActiveTexture(GL_TEXTURE0 + ambientIndex);
            textures[ambientIndex]->texture.bind();
            shader->setUniformValue("texture_diffuse1", ambientIndex);
        }
        else if (!haveAmbient)
        {
            glFunc->glActiveTexture(GL_TEXTURE0 + diffuseIndex);
            textures[diffuseIndex]->texture.bind();
            shader->setUniformValue("texture_ambient1", diffuseIndex);
        }
    }

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
