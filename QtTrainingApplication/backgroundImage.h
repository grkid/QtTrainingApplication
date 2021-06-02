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
#include <QDir>
#include <QImage>
#include <QWidget>
#include <QSize>

class BackgroundImage
{
public:
	void draw();
	//void destroy();
	BackgroundImage(QString path, QOpenGLWidget* widget);
	~BackgroundImage();
	QSize getSize();

	//可重入函数
	void readBackground(QString path);

	const int textureIndex = 14;

private:
	//~BackgroundImage();
	QOpenGLWidget* widget;
	QVector<float> vertices;
	QImage image;
	QOpenGLShaderProgram shaderProgram;
	QOpenGLBuffer VBO;
	QOpenGLVertexArrayObject VAO;
	QOpenGLTexture texture;
};

