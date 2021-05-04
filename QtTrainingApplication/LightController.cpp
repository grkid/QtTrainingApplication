#include "LightController.h"
#include "MainWindow.h"

LightController::LightController(QWidget* parent)
{
	setMaximumWidth(250);
	setMinimumWidth(250);
	degXZLabel = new QLabel();
	//degXZLabel->setText(TR("水平角：") + degXZ + TR("度"));
	//setText();
	labels.append(degXZLabel);

	degYLabel = new QLabel();
	//degYLabel->setText(TR("垂直角：") + degY + TR("度"));
	labels.append(degYLabel);

	redColorLabel = new QLabel();
	//redColorLabel->setText(TR("红色值：") + r);
	labels.append(redColorLabel);

	greenColorLabel = new QLabel();
	//greenColorLabel->setText(TR("绿色值：") + g);
	labels.append(greenColorLabel);

	blueColorLabel = new QLabel();
	//blueColorLabel->setText(TR("蓝色值：") + b);
	labels.append(blueColorLabel);

	ambientTensityLabel = new QLabel();
	//ambientTensityLabel->setText(TR("环境光强度：") + ambientTensity);
	labels.append(ambientTensityLabel);

	diffuseTensityLabel = new QLabel();
	//diffuseTensityLabel->setText(TR("漫反射强度：") + diffuseTensity);
	labels.append(diffuseTensityLabel);

	specularTensityLabel = new QLabel();
	//specularTensityLabel->setText(TR("镜面强度") + specularTensity);
	labels.append(specularTensityLabel);

	degXZSlider = new QSlider();
	degXZSlider->setMinimum(0); degXZSlider->setMaximum(360);
	degXZSlider->setValue(degXZ);
	sliders.append(degXZSlider);

	degYSlider = new QSlider();
	degYSlider->setMinimum(-90); degYSlider->setMaximum(90);
	degYSlider->setValue(degY);
	sliders.append(degYSlider);

	redColorSlider = new QSlider();
	redColorSlider->setMinimum(0); redColorSlider->setMaximum(255);
	redColorSlider->setValue(r * 255);
	sliders.append(redColorSlider);

	greenColorSlider = new QSlider();
	greenColorSlider->setMinimum(0); greenColorSlider->setMaximum(255);
	greenColorSlider->setValue(g * 255);
	sliders.append(greenColorSlider);

	blueColorSlider = new QSlider();
	blueColorSlider->setMinimum(0); blueColorSlider->setMaximum(255);
	blueColorSlider->setValue(b * 255);
	sliders.append(blueColorSlider);

	ambientTensitySlider = new QSlider();
	ambientTensitySlider->setMinimum(0); ambientTensitySlider->setMaximum(tensityInterval);
	ambientTensitySlider->setValue(ambientTensity * tensityInterval);
	sliders.append(ambientTensitySlider);

	diffuseTensitySlider = new QSlider();
	diffuseTensitySlider->setMinimum(0); diffuseTensitySlider->setMaximum(tensityInterval);
	diffuseTensitySlider->setValue(diffuseTensity * tensityInterval);
	sliders.append(diffuseTensitySlider);

	specularTensitySlider = new QSlider();
	specularTensitySlider->setMinimum(0); specularTensitySlider->setMaximum(tensityInterval);
	specularTensitySlider->setValue(specularTensity * tensityInterval);
	sliders.append(specularTensitySlider);

	demoCheckBox = new QCheckBox(TR("开启演示模式"), this);
	demoCheckBox->setCheckState(Qt::Unchecked);

	floorTransparentCheckBox = new QCheckBox(TR("透明底板"), this);
	floorTransparentCheckBox->setCheckState(Qt::Unchecked);

	set();
}

LightController::~LightController()
{
	//TODO
}

void LightController::set()
{
	setText();

	for (auto item : sliders)
	{
		item->setOrientation(Qt::Horizontal);
	}
	connect(degXZSlider, &QSlider::valueChanged,
		[=]()
		{
			degXZ = degXZSlider->value();
			//degXZLabel->setText(TR("水平角：") + degXZ + TR("度"));
			setText();
			MainWindow::getOpenGLWidget()->directionalLight->setDirection(degXZ, degY);
		}
	);

	connect(degYSlider, &QSlider::valueChanged,
		[=]()
		{
			degY = degYSlider->value();
			//degYLabel->setText(TR("垂直角：") + degXZ + TR("度"));
			setText();
			MainWindow::getOpenGLWidget()->directionalLight->setDirection(degXZ, degY);
		}
	);

	connect(redColorSlider, &QSlider::valueChanged,
		[=]()
		{
			r = redColorSlider->value() / 255.0;
			setText();
			MainWindow::getOpenGLWidget()->directionalLight->setColor(r, g, b);
		}
	);

	connect(greenColorSlider, &QSlider::valueChanged,
		[=]()
		{
			g = greenColorSlider->value() / 255.0;
			setText();
			MainWindow::getOpenGLWidget()->directionalLight->setColor(r, g, b);
		}
	);

	connect(blueColorSlider, &QSlider::valueChanged,
		[=]()
		{
			b = blueColorSlider->value() / 255.0;
			setText();
			MainWindow::getOpenGLWidget()->directionalLight->setColor(r, g, b);
		}
	);

	connect(ambientTensitySlider, &QSlider::valueChanged,
		[=]()
		{
			ambientTensity = ambientTensitySlider->value() / (float)(tensityInterval);
			setText();
			MainWindow::getOpenGLWidget()->directionalLight->setAmbientTensity(ambientTensity);
		}
	);

	connect(diffuseTensitySlider, &QSlider::valueChanged,
		[=]()
		{
			diffuseTensity = diffuseTensitySlider->value() / (float)(tensityInterval);
			setText();
			MainWindow::getOpenGLWidget()->directionalLight->setDiffuseTensity(diffuseTensity);
		}
	);

	connect(specularTensitySlider, &QSlider::valueChanged,
		[=]()
		{
			specularTensity = specularTensitySlider->value() / (float)(tensityInterval);
			setText();
			MainWindow::getOpenGLWidget()->directionalLight->setSpecularTensity(specularTensity);
		}
	);

	connect(demoCheckBox, &QCheckBox::stateChanged,
		[=]()
		{
			if (demoCheckBox->checkState() == Qt::Checked)
			{
				MainWindow::getOpenGLWidget()->setDemo(1);
			}
			else // unchecked
			{
				MainWindow::getOpenGLWidget()->setDemo(0);
			}
		}
		);

	connect(floorTransparentCheckBox, &QCheckBox::stateChanged,
		[=]()
		{
			if (floorTransparentCheckBox->checkState() == Qt::Checked)
			{
				MainWindow::getOpenGLWidget()->setFloorTransparent(1);
			}
			else // unchecked
			{
				MainWindow::getOpenGLWidget()->setFloorTransparent(0);
			}
		}
	);

	layout = new QGridLayout();
	for (int i = 0; i < labels.size(); i++)
	{
		layout->addWidget(labels[i], i, 0);
		layout->addWidget(sliders[i], i, 1);
	}
	layout->addWidget(demoCheckBox, labels.size(), 1);
	layout->addWidget(floorTransparentCheckBox, labels.size()+1, 1);
	this->setLayout(layout);
	this->show();

}

void LightController::setText()
{
	std::strstream stream;
	std::string str;

	stream<< "水平角：" << degXZ << "度";
	stream >> str;
	degXZLabel->setText(TR(str));

	stream.clear();
	str.clear();
	stream << "垂直角：" << degY << "度";
	stream >> str;
	degYLabel->setText(TR(str));

	stream.clear();
	str.clear();
	stream << "红色值：" << r;
	stream >> str;
	redColorLabel->setText(TR(str));

	stream.clear();
	str.clear();
	stream << "蓝色值：" << b;
	stream >> str;
	blueColorLabel->setText(TR(str));

	stream.clear();
	str.clear();
	stream << "绿色值：" << g;
	stream >> str;
	greenColorLabel->setText(TR(str));

	stream.clear();
	str.clear();
	stream << "环境光强度：" << ambientTensity;
	stream >> str;
	ambientTensityLabel->setText(TR(str));

	stream.clear();
	str.clear();
	stream << "漫反射强度：" << diffuseTensity;
	stream >> str;
	diffuseTensityLabel->setText(TR(str));

	stream.clear();
	str.clear();
	stream << "alpha：" << specularTensity;
	stream >> str;
	specularTensityLabel->setText(TR(str));


}
