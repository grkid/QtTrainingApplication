#ifndef WIDGET_H
#define WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QTimer>
#include <QTime>
#include <QtMath>
#include <QMouseEvent>
class Widget : public QOpenGLWidget, public QOpenGLExtraFunctions
{
	Q_OBJECT

public:
	Widget(QWidget *parent = 0);
	~Widget();
protected:
	virtual void initializeGL() override;
	virtual void resizeGL(int w, int h) override;
	virtual void paintGL() override;

	virtual void mouseMoveEvent(QMouseEvent* event) override;
private:
	QVector<float> vertices;
	QOpenGLShaderProgram shaderProgram;
	QOpenGLBuffer VBO;
	QOpenGLVertexArrayObject VAO;
	QOpenGLTexture texture;
	QOpenGLTexture texture1;
	QTimer timer;
	QVector<QVector3D> cubePositions;

	double yaw;             //∆´∫ΩΩ«
	double pitch;           //∏© ”Ω«
	double sensitivity;     // Û±Í¡È√Ù∂»

	QVector3D cameraPos;
	QVector3D cameraTarget;
	QVector3D cameraDirection;
	QVector3D cameraRight;
	QVector3D cameraUp;
};

#endif // WIDGET_H