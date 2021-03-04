#include "widget.h"
#include <QtMath>

Widget::Widget(QWidget *parent)
	: QOpenGLWidget(parent)
	, VBO(QOpenGLBuffer::VertexBuffer)
	, texture(QOpenGLTexture::Target2D)
	, texture1(QOpenGLTexture::Target2D)
	, yaw(0.0)
	, pitch(0.0)
	, sensitivity(0.01)
	, cameraPos(-5.0f, 0.0f, 0.0f)
	, cameraTarget(0.0f, 0.0f, 0.0f)
	, cameraDirection(cos(yaw)* cos(pitch), sin(pitch), sin(yaw)* cos(pitch))
	, cameraRight(QVector3D::crossProduct({ 0.0f,1.0f,0.0f }, cameraDirection))
	, cameraUp(QVector3D::crossProduct(cameraDirection, cameraRight))
{
	vertices = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	cubePositions = {
	  { 0.0f,  0.0f,  0.0f  },
	  { 2.0f,  5.0f, -15.0f },
	  {-1.5f, -2.2f, -2.5f  },
	  {-3.8f, -2.0f, -12.3f },
	  { 2.4f, -0.4f, -3.5f  },
	  {-1.7f,  3.0f, -7.5f  },
	  { 1.3f, -2.0f, -2.5f  },
	  { 1.5f,  2.0f, -2.5f  },
	  { 1.5f,  0.2f, -1.5f  },
	  {-1.3f,  1.0f, -1.5f  },
	};

	timer.setInterval(18);
	connect(&timer, &QTimer::timeout, this, static_cast<void (Widget::*)()>(&Widget::update));
	timer.start();

	setMouseTracking(true);         //�������׷�٣�QtĬ�ϲ���ʵʱ�������ƶ�
}

Widget::~Widget()
{
}

void Widget::initializeGL()
{
	this->initializeOpenGLFunctions();        //��ʼ��opengl����
	if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "./shader/1.glsl")) {     //��Ӳ����붥����ɫ��
		qDebug() << "ERROR:" << shaderProgram.log();    //����������,��ӡ������Ϣ
	}
	if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "./shader/2.glsl")) {   //��Ӳ�����Ƭ����ɫ��
		qDebug() << "ERROR:" << shaderProgram.log();    //����������,��ӡ������Ϣ
	}
	if (!shaderProgram.link()) {                      //������ɫ��
		qDebug() << "ERROR:" << shaderProgram.log();    //������ӳ���,��ӡ������Ϣ
	}

	QOpenGLVertexArrayObject::Binder{ &VAO };

	VBO.create();       //����VBO����
	VBO.bind();         //��VBO�󶨵���ǰ�Ķ��㻺�����QOpenGLBuffer::VertexBuffer����

	//���������ݷ��䵽VBO�У���һ������Ϊ����ָ�룬�ڶ�������Ϊ���ݵ��ֽڳ���
	VBO.allocate(vertices.data(), sizeof(float)*vertices.size());

	texture.create();
	texture.setData(QImage("./image/container.jpg").mirrored());
	texture.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
	texture.setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
	texture.setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);

	texture1.create();
	texture1.setData(QImage("./image/awesomeface.png").mirrored());
	texture1.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
	texture1.setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
	texture1.setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);

	//���ö��������ʽ�������ö���
	shaderProgram.setAttributeBuffer("aPos", GL_FLOAT, 0, 3, sizeof(GLfloat) * 5);
	shaderProgram.enableAttributeArray("aPos");
	shaderProgram.setAttributeBuffer("aTexCoord", GL_FLOAT, sizeof(GLfloat) * 3, 2, sizeof(GLfloat) * 5);
	shaderProgram.enableAttributeArray("aTexCoord");

	this->glEnable(GL_DEPTH_TEST);
	this->setCursor(Qt::BlankCursor);       //���������
	QCursor::setPos(geometry().center());   //�������λ��Ϊ���ھ������������
}

void Widget::resizeGL(int w, int h)
{
	this->glViewport(0, 0, w, h);                //�����ӿ�����
}

void Widget::paintGL()
{
	this->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  //����������ɫ
	this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);       //�����ɫ����



	shaderProgram.bind();                     //ʹ��shaderProgram��ɫ����
	{

		float time = QTime::currentTime().msecsSinceStartOfDay() / 1000.0;

		QMatrix4x4 view;
		view.lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
		shaderProgram.setUniformValue("view", view);

		QMatrix4x4 projection;
		projection.perspective(45.0f, width() / (float)height(), 0.1f, 100.0f);
		shaderProgram.setUniformValue("projection", projection);

		texture.bind(0);                                    //��texture�󶨵�����Ԫ0
		shaderProgram.setUniformValue("ourTexture", 0);      //��ourTexture������Ԫ0�л�ȡ��������

		texture1.bind(1);                                    //��texture�󶨵�����Ԫ1
		shaderProgram.setUniformValue("ourTexture1", 1);      //��ourTexture������Ԫ1�л�ȡ��������

		QOpenGLVertexArrayObject::Binder{ &VAO };
		
		for (unsigned int i = 0; i < 10; i++) {
			QMatrix4x4 model;
			model.translate(cubePositions[i]);
			model.rotate(180 * time + i * 20.0f, QVector3D(1.0f, 0.5f, 0.3f));
			shaderProgram.setUniformValue("model", model);
			this->glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
}

void Widget::mouseMoveEvent(QMouseEvent* event)
{
	float xoffset = event->x() - rect().center().x();
	float yoffset = rect().center().y() - event->y(); // ע���������෴�ģ���Ϊy�����Ǵӵײ����������������
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	yaw += xoffset;
	pitch += yoffset;
	if (pitch >= M_PI / 2)                          //�����ӽ����Ƶ�[-90��,90��]
		pitch = (M_PI) / 2 - 0.1;
	if (pitch <= -M_PI / 2)
		pitch = -(M_PI) / 2 + 0.1;
	cameraDirection.setX(cos(yaw) * cos(pitch));
	cameraDirection.setY(sin(pitch));
	cameraDirection.setZ(sin(yaw) * cos(pitch));
	QCursor::setPos(geometry().center());       //����긴ԭ����������
}