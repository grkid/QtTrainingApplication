#pragma once
#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <qmath.h>
class DirectionalLight
{
public:
    DirectionalLight();
    void set(QOpenGLShaderProgram* shader);

    //传入-1即为无效值，不设置
    void setDirection(float x, float y, float z);
    void setDirection(float degXZ, float degY);
    void setDirection(QVector3D d);
    QVector3D getDirection();
    void setAmbientTensity(float value);
    float getAmbientTensity();
    void setDiffuseTensity(float value);
    float getDiffuseTensity();
    void setSpecularTensity(float value);
    float getSpecularTensity();
    void setColor(float r, float g, float b);
    void setColor(QVector3D c);
    QVector3D getColor();

private:
    float ambientTensity = 1;
    float diffuseTensity = 1;
    float specularTensity = 1;
    QVector3D direction;
    QVector3D rgbColor;



};

