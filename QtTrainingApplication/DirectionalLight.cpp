#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
	direction = QVector3D(-0.2, -1.0, -0.3);
	rgbColor = QVector3D(1.0, 1.0, 1.0);
	ambientTensity = 0.6;
	diffuseTensity = 0.5;
	specularTensity = 1;
}

void DirectionalLight::set(QOpenGLShaderProgram* shader)
{
	shader->bind();

	shader->setUniformValue("dLight.direction",direction);
	shader->setUniformValue("dLight.ambient", ambientTensity*rgbColor);
	shader->setUniformValue("dLight.diffuse", diffuseTensity*rgbColor);
	shader->setUniformValue("dLight.specular", specularTensity*rgbColor);
}

void DirectionalLight::setDirection(float x, float y, float z)
{
	if (x == -1.0)
		x = direction.x();
	if (y == -1.0)
		y = direction.y();
	if (z == -1.0)
		z = direction.z();

	direction = QVector3D(x, y, z);
}

void DirectionalLight::setDirection(float degXZ, float degY)
{
	float radXZ = (M_PI * degXZ) / 180;
	float radY = (M_PI * degY) / 180;
	setDirection(cos(radXZ) * cos(radY), sin(radY), sin(radXZ) * cos(radY));
}

QVector3D DirectionalLight::getDirection()
{
	return direction;
}

void DirectionalLight::setAmbientTensity(float value)
{
	ambientTensity = value;
}

float DirectionalLight::getAmbientTensity()
{
	return ambientTensity;
}

void DirectionalLight::setDiffuseTensity(float value)
{
	diffuseTensity = value;
}

float DirectionalLight::getDiffuseTensity()
{
	return diffuseTensity;
}

void DirectionalLight::setSpecularTensity(float value)
{
	specularTensity = value;
}

float DirectionalLight::getSpecularTensity()
{
	return specularTensity;
}

void DirectionalLight::setColor(float r, float g, float b)
{
	if (r == -1.0)
		r = rgbColor.x();
	if (g == -1.0)
		g = rgbColor.y();
	if (b == -1.0)
		b = rgbColor.z();

	rgbColor = QVector3D(r, g, b);
}

QVector3D DirectionalLight::getColor()
{
	return rgbColor;
}
