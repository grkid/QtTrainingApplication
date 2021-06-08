#include "ModelController.h"
#include "LanguageAdapter.h"
#include "MainWindow.h"

ModelController::ModelController(QWidget* parent)
{
	setMaximumWidth(250);
	setMinimumWidth(250);

	leftButton = new QPushButton();
	leftButton->setText(TR("����"));
	buttonVector.append(leftButton);

	rightButton = new QPushButton();
	rightButton->setText(TR("����"));
	buttonVector.append(rightButton);

	upButton = new QPushButton();
	upButton->setText(TR("����"));
	buttonVector.append(upButton);

	downButton = new QPushButton();
	downButton->setText(TR("����"));
	buttonVector.append(downButton);

	frontButton = new QPushButton();
	frontButton->setText(TR("ǰ��"));
	buttonVector.append(frontButton);

	backButton = new QPushButton();
	backButton->setText(TR("����"));
	buttonVector.append(backButton);

	rotateXCButton = new QPushButton();
	rotateXCButton->setText(TR("��X��˳ʱ����ת"));
	buttonVector.append(rotateXCButton);

	rotateXAButton = new QPushButton();
	rotateXAButton->setText(TR("��X����ʱ����ת"));
	buttonVector.append(rotateXAButton);

	rotateYCButton = new QPushButton();
	rotateYCButton->setText(TR("��Y��˳ʱ����ת"));
	buttonVector.append(rotateYCButton);

	rotateYAButton = new QPushButton();
	rotateYAButton->setText(TR("��Y����ʱ����ת"));
	buttonVector.append(rotateYAButton);

	rotateZCButton = new QPushButton();
	rotateZCButton->setText(TR("��Z��˳ʱ����ת"));
	buttonVector.append(rotateZCButton);

	rotateZAButton = new QPushButton();
	rotateZAButton->setText(TR("��Z����ʱ����ת"));
	buttonVector.append(rotateZAButton);

	scaleUpButton = new QPushButton();
	scaleUpButton->setText(TR("�Ŵ�"));
	buttonVector.append(scaleUpButton);

	scaleDownButton = new QPushButton();
	scaleDownButton->setText(TR("��С"));
	buttonVector.append(scaleDownButton);

	comboBox = new QComboBox();

	set();
}

ModelController::~ModelController()
{

}

void ModelController::addmodelName(QString str)
{
	modelNameList.append(str);
	comboBox->addItem(str);
}

void ModelController::clearModelName()
{
	modelNameList.clear();
	comboBox->clear();
}

void ModelController::set()
{
	for (auto item : buttonVector)
	{
		item->setAutoRepeat(true);
		item->setAutoRepeatDelay(16);
		item->setAutoRepeatInterval(16);
	}

	//TODO:����modelName

	connect(leftButton, &QPushButton::clicked,
		[=]()
		{
			MainWindow::getOpenGLWidget()->modelTransform(selectedModelName, modelTranslate, tLeft);
		}
	);

	connect(rightButton, &QPushButton::clicked,
		[=]()
		{
			MainWindow::getOpenGLWidget()->modelTransform(selectedModelName, modelTranslate, tRight);
		}
	);

	connect(upButton, &QPushButton::clicked,
		[=]()
		{
			MainWindow::getOpenGLWidget()->modelTransform(selectedModelName, modelTranslate, tUp);
		}
	);

	connect(downButton, &QPushButton::clicked,
		[=]()
		{
			MainWindow::getOpenGLWidget()->modelTransform(selectedModelName, modelTranslate, tDown);
		}
	);

	connect(frontButton, &QPushButton::clicked,
		[=]()
		{
			MainWindow::getOpenGLWidget()->modelTransform(selectedModelName, modelTranslate, tFront);
		}
	);

	connect(backButton, &QPushButton::clicked,
		[=]()
		{
			MainWindow::getOpenGLWidget()->modelTransform(selectedModelName, modelTranslate, tBack);
		}
	);

	connect(rotateXCButton, &QPushButton::clicked,
		[=]()
		{
			MainWindow::getOpenGLWidget()->modelTransform(selectedModelName, modelRotate, rXc);
		}
	);

	connect(rotateXAButton, &QPushButton::clicked,
		[=]()
		{
			MainWindow::getOpenGLWidget()->modelTransform(selectedModelName, modelRotate, rXa);
		}
	);

	connect(rotateYCButton, &QPushButton::clicked,
		[=]()
		{
			MainWindow::getOpenGLWidget()->modelTransform(selectedModelName, modelRotate, rYc);
		}
	);

	connect(rotateYAButton, &QPushButton::clicked,
		[=]()
		{
			MainWindow::getOpenGLWidget()->modelTransform(selectedModelName, modelRotate, rYa);
		}
	);

	connect(rotateZCButton, &QPushButton::clicked,
		[=]()
		{
			MainWindow::getOpenGLWidget()->modelTransform(selectedModelName, modelRotate, rZc);
		}
	);

	connect(rotateZAButton, &QPushButton::clicked,
		[=]()
		{
			MainWindow::getOpenGLWidget()->modelTransform(selectedModelName, modelRotate, rZa);
		}
	);

	connect(scaleUpButton, &QPushButton::clicked,
		[=]()
	{
		MainWindow::getOpenGLWidget()->modelTransform(selectedModelName, modelScale, sUp);
	}
	);

	connect(scaleDownButton, &QPushButton::clicked,
		[=]()
	{
		MainWindow::getOpenGLWidget()->modelTransform(selectedModelName, modelScale, sDown);
	}
	);

	connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
    [=](int index){ 
		selectedModelName = comboBox->itemText(index);
		qDebug() << "selected model name:" + selectedModelName;
	}
	);

	layout = new QVBoxLayout();
	layout->addWidget(comboBox);
	for (auto item : buttonVector) {
		layout->addWidget(item);
	}
	this->setLayout(layout);
	this->show();
}