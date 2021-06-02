#include "backgroundImage.h"

void BackgroundImage::draw()
{
    //widget->context()->functions()->glClearColor(0.1f, 0.5f, 0.7f, 1.0f);  //设置清屏颜色
    //widget->context()->functions()->glClear(GL_COLOR_BUFFER_BIT);       //清除颜色缓存
    shaderProgram.bind();                     //使用shaderProgram着色程序
    
    const int textureIndex = 14;
    texture.bind(textureIndex);                                    //将texture绑定到纹理单元0

    //debug代码，用于查看深度图
    shaderProgram.setUniformValue("Texture", 14);      //让着色采样器ourTexture从纹理单元0中获取纹理数据
    QOpenGLVertexArrayObject::Binder{ &VAO };
    widget->context()->functions()->glDrawArrays(GL_POLYGON, 0, 4);     //使用以0开始，长度为4的顶点数据来绘制多边形
    //VAO.release();
    
}

BackgroundImage::BackgroundImage(QString path, QOpenGLWidget* widget)
    : VBO(QOpenGLBuffer::VertexBuffer)
    , texture(QOpenGLTexture::Target2D)
{
    const float _zmax=0.9999999f;
    vertices = {
        // 位置                            //纹理坐标
         1.0f, -1.0f, _zmax,    1.0f, 0.0f,   // 右下
        -1.0f, -1.0f, _zmax,     0.0f, 0.0f,   // 左下
        -1.0f,  1.0f, _zmax,    0.0f, 1.0f,   // 左下
         1.0f,  1.0f, _zmax,     1.0f, 1.0f,   // 右上
    };

    this->widget = widget;

    if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "shader/background.vert")) {     //添加并编译顶点着色器
        qDebug() << "ERROR:" << shaderProgram.log();    //如果编译出错,打印报错信息
    }
    if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "shader/background.frag")) {   //添加并编译片段着色器
        qDebug() << "ERROR:" << shaderProgram.log();    //如果编译出错,打印报错信息
    }
    if (!shaderProgram.link()) {                      //链接着色器
        qDebug() << "ERROR:" << shaderProgram.log();    //如果链接出错,打印报错信息
    }

    QOpenGLVertexArrayObject::Binder{ &VAO };

    VBO.create();       //生成VBO对象
    VBO.bind();         //将VBO绑定到当前的顶点缓冲对象（QOpenGLBuffer::VertexBuffer）中

    //将顶点数据分配到VBO中，第一个参数为数据指针，第二个参数为数据的字节长度
    VBO.allocate(vertices.data(), sizeof(float) * vertices.size());
    texture.create();
    image = QImage(path);
    texture.setData(image.mirrored());
    texture.setAutoMipMapGenerationEnabled(FALSE);//防止背景模糊
    //texture.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
    texture.setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
    texture.setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);

    //设置顶点解析格式，并启用顶点
    shaderProgram.setAttributeBuffer("aPos", GL_FLOAT, 0, 3, sizeof(GLfloat) * 5);
    shaderProgram.enableAttributeArray("aPos");
    shaderProgram.setAttributeBuffer("aTexCoord", GL_FLOAT, sizeof(GLfloat) * 3, 2, sizeof(GLfloat) * 5);
    shaderProgram.enableAttributeArray("aTexCoord");
}

BackgroundImage::~BackgroundImage()
{
    //暂时没有需要清理的
}

QSize BackgroundImage::getSize()
{
    return image.size();
}

void BackgroundImage::readBackground(QString path)
{
    image = QImage(path);
    texture.setData(image.mirrored());
}


