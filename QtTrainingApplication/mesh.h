#pragma once
#include <QString>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLTexture>
#include <QOpenGLWidget>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
struct Vertex
{
    QVector3D Position;
    QVector3D Normal;
    QVector2D TexCoords;
    QVector3D Tangent;
    QVector3D Bitangent;
};

struct Texture
{
    QOpenGLTexture texture;
    QString type;
    QString path;
    Texture() :texture(QOpenGLTexture::Target2D) {
        texture.create();
        texture.setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
        texture.setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
        texture.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
    }
};

class Mesh {
public:
    /*  ��������  */
    QVector<Vertex> vertices;               //��������
    QVector<unsigned int> indices;          //��������
    QVector<Texture*> textures;             //��������
    QMatrix4x4 model;                       //ģ�;���
    QOpenGLFunctions* glFunc;               //opengl�������

    /*  ����  */
    Mesh(QOpenGLFunctions* glFunc, aiMatrix4x4 model);
    void draw(QOpenGLShaderProgram* shader);
    void setupMesh();

private:
    /*  ��Ⱦ����  */
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VBO, EBO;
};