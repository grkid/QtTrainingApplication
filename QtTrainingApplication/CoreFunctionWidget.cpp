#include "CoreFunctionWidget.h"
#include <QDebug>
#include <QFile>
#include <QTime>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//
//#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp"
//#include "glm/gtc/type_ptr.hpp"

#include <QMatrix4x4>
#include <QtMath>

static GLuint VBO, VAO, EBO;
static GLuint texture1,texture2;

CoreFunctionWidget::CoreFunctionWidget(QWidget *parent) : QOpenGLWidget(parent)
{

}

CoreFunctionWidget::~CoreFunctionWidget()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//    glDeleteBuffers(1, &EBO);
}

void CoreFunctionWidget::initializeGL() {
	this->initializeOpenGLFunctions();

	bool success = shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "./shader/1.glsl");
	if (!success) {
		qDebug() << "shaderProgram addShaderFromSourceFile failed!" << shaderProgram.log();
		return;
	}

	success = shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "./shader/2.glsl");
	if (!success) {
		qDebug() << "shaderProgram addShaderFromSourceFile failed!" << shaderProgram.log();
		return;
	}

	success = shaderProgram.link();
	if (!success) {
		qDebug() << "shaderProgram link failed!" << shaderProgram.log();
	}

	//VAO，VBO数据部分
	float vertices[] = {
		//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  //顶点数据复制到缓冲

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);//取消VBO的绑定, glVertexAttribPointer已经把顶点属性关联到顶点缓冲对象了

//    remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

//    You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
//    VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);   //取消VAO绑定
	
	//纹理部分
	int textureWidth, textureHeight, textureNrChannels;
	unsigned char *data = stbi_load("./image/container.jpg", &textureWidth, &textureHeight, &textureNrChannels, 0);


	glGenTextures(1, &texture1);
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("./image/awesomeface.png", &textureWidth, &textureHeight, &textureNrChannels, 0);
	glGenTextures(1, &texture2);
	//glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	//设置纹理uniform
	shaderProgram.bind();
	shaderProgram.setUniformValue("texture1", 0);
	shaderProgram.setUniformValue("texture2", 1);
}

void CoreFunctionWidget::resizeGL(int w, int h) {
	glViewport(0, 0, w, h);
}

void CoreFunctionWidget::paintGL() {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	shaderProgram.bind();
	{
		//旋转矩阵
		float time = QTime::currentTime().msecsSinceStartOfDay() / 1000.0;
		QMatrix4x4 trans;
		trans.translate(0.0f, 0.5*qAbs(qSin(time)), 0.0f);        //向y轴平移0.5*[0,1]
		trans.scale(0.5*qAbs(qSin(time)), 0.5*qAbs(qSin(time))); //x，y在[0,0.5]进行缩放
		trans.rotate(360 * time, 0.0f, 0.0f, -1.0f);                 //旋转360*time
		shaderProgram.setUniformValue("transform", trans);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	//shaderProgram.release();
}
