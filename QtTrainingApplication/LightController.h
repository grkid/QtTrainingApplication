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
	//TODO:��Դ�������
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

	float degXZ = 0, degY = 0;	//�Ƕ�
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
* ��Դ�������ԣ�
* XZƽ��Ƕȣ���X��������Ϊ0�ȣ�˳ʱ��������ֵ��0��360����λ�ȣ�����degXZ
* Y��нǣ���XZƽ��Ϊ0�ȣ���Y�������������������С��ֵ��-90��90����λ�ȣ�����degY
* �������нǵõ�����������
* (cos(degXZ)*cos(degY),sin(degY),sin(degXZ)cos(degY))
*/
