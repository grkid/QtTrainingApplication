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
	QImage image;//�̶�һ��ȫ͸��PNG
	QOpenGLShaderProgram* shaderProgram;	//��Ⱦ��ɫ��
	QOpenGLShaderProgram* shadowShaderProgram;	//��Ӱ��ɫ��

	//�������
	QOpenGLBuffer VBO;
	QOpenGLVertexArrayObject VAO;
	QOpenGLTexture texture;

};

