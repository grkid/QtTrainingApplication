#pragma once
#include <QImage>
#include <QWidget>
#include <QVector>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include "mesh.h"
class Shadow
{
public:
	void draw();
	Shadow(QOpenGLWidget* widget,QOpenGLShaderProgram* shader);
private:
	QOpenGLWidget* widget;
	QVector<float> vertices;
	QImage image;//固定一张全透明PNG
	QOpenGLShaderProgram* shaderProgram;	//渲染着色器
	QOpenGLShaderProgram* shadowShaderProgram;	//阴影着色器

	//绘制相关
	QOpenGLBuffer VBO;
	QOpenGLVertexArrayObject VAO;
	QOpenGLTexture texture;

};

