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

	setMouseTracking(true);         //开启鼠标追踪：Qt默认不会实时监控鼠标移动
}

Widget::~Widget()
{
}

void Widget::initializeGL()
{
	this->initializeOpenGLFunctions();        //初始化opengl函数
	if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "./shader/1.glsl")) {     //添加并编译顶点着色器
		qDebug() << "ERROR:" << shaderProgram.log();    //如果编译出错,打印报错信息
	}
	if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "./shader/2.glsl")) {   //添加并编译片段着色器
		qDebug() << "ERROR:" << shaderProgram.log();    //如果编译出错,打印报错信息
	}
	if (!shaderProgram.link()) {                      //链接着色器
		qDebug() << "ERROR:" << shaderProgram.log();    //如果链接出错,打印报错信息
	}

	QOpenGLVertexArrayObject::Binder{ &VAO };

	VBO.create();       //生成VBO对象
	VBO.bind();         //将VBO绑定到当前的顶点缓冲对象（QOpenGLBuffer::VertexBuffer）中

	//将顶点数据分配到VBO中，第一个参数为数据指针，第二个参数为数据的字节长度
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

	//设置顶点解析格式，并启用顶点
	shaderProgram.setAttributeBuffer("aPos", GL_FLOAT, 0, 3, sizeof(GLfloat) * 5);
	shaderProgram.enableAttributeArray("aPos");
	shaderProgram.setAttributeBuffer("aTexCoord", GL_FLOAT, sizeof(GLfloat) * 3, 2, sizeof(GLfloat) * 5);
	shaderProgram.enableAttributeArray("aTexCoord");

	this->glEnable(GL_DEPTH_TEST);
	this->setCursor(Qt::BlankCursor);       //隐藏鼠标光标
	QCursor::setPos(geometry().center());   //设置鼠标位置为窗口矩形区域的中心
}

void Widget::resizeGL(int w, int h)
{
	this->glViewport(0, 0, w, h);                //定义视口区域
}

void Widget::paintGL()
{
	this->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  //设置清屏颜色
	this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);       //清除颜色缓存



	shaderProgram.bind();                     //使用shaderProgram着色程序
	{

		float time = QTime::currentTime().msecsSinceStartOfDay() / 1000.0;

		QMatrix4x4 view;
		view.lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
		shaderProgram.setUniformValue("view", view);

		QMatrix4x4 projection;
		projection.perspective(45.0f, width() / (float)height(), 0.1f, 100.0f);
		shaderProgram.setUniformValue("projection", projection);

		texture.bind(0);                                    //将texture绑定到纹理单元0
		shaderProgram.setUniformValue("ourTexture", 0);      //让ourTexture从纹理单元0中获取纹理数据

		texture1.bind(1);                                    //将texture绑定到纹理单元1
		shaderProgram.setUniformValue("ourTexture1", 1);      //让ourTexture从纹理单元1中获取纹理数据

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
	float yoffset = rect().center().y() - event->y(); // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	yaw += xoffset;
	pitch += yoffset;
	if (pitch >= M_PI / 2)                          //将俯视角限制到[-90°,90°]
		pitch = (M_PI) / 2 - 0.1;
	if (pitch <= -M_PI / 2)
		pitch = -(M_PI) / 2 + 0.1;
	cameraDirection.setX(cos(yaw) * cos(pitch));
	cameraDirection.setY(sin(pitch));
	cameraDirection.setZ(sin(yaw) * cos(pitch));
	QCursor::setPos(geometry().center());       //将鼠标复原到窗口中央
}