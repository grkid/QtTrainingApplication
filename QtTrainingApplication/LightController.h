#pragma once
#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QVector>
#include <QGridLayout>
#include "LanguageAdapter.h"
#include <string>

class LightController:public QWidget
{
	//TODO:光源控制面板
	Q_OBJECT
public:
	LightController(QWidget* parent = NULL);
	~LightController();

private:
	QLabel* degXZLabel, * degYLabel;
	QLabel* redColorLabel, * greenColorLabel, * blueColorLabel;
	QLabel* ambientTensityLabel, *diffuseTensityLabel, *specularTensityLabel;

	QSlider* degXZSlider, *degYSlider;
	QSlider* redColorSlider, * greenColorSlider, *blueColorSlider;
	QSlider* ambientTensitySlider, * diffuseTensitySlider, *specularTensitySlider;

	float degXZ = 0, degY = 0;	//角度
	float ambientTensity = 1.0, diffuseTensity = 1.0, specularTensity = 1.0;
	float r = 1.0, g = 1.0, b = 1.0;

	int tensityInterval = 1000;

	QVector<QLabel*> labels;
	QVector <QSlider*> sliders;

	QGridLayout* layout;

	void set();
	void setText();
};



/*
* 光源调整策略：
* XZ平面角度，以X轴正方向为0度，顺时针增长，值域0到360，单位度，名称degXZ
* Y轴夹角，以XZ平面为0度，向Y正方向增长，反方向减小，值域-90到90，单位度，名称degY
* 从两个夹角得到方向向量：
* (cos(degXZ)*cos(degY),sin(degY),sin(degXZ)cos(degY))
*/
