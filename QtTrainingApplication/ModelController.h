#pragma once
#include <QWidget>
#include <QtWidgets/qpushbutton.h>
#include <QComboBox>
#include <QVector>
#include <QVBoxLayout>
#include "LanguageAdapter.h"
#include "model.h"
class ModelController:public QWidget
{
	Q_OBJECT

public:
	ModelController(QWidget* parent = NULL);
	~ModelController();

	void addmodelName(QString str);
	void clearModelName();

private:
	QPushButton* leftButton, *rightButton, *upButton, *downButton, *frontButton, *backButton;
	QPushButton* rotateXCButton, *rotateXAButton, *rotateYCButton, *rotateYAButton, *rotateZCButton, *rotateZAButton;
	QPushButton* scaleUpButton, *scaleDownButton;
	QVector<QPushButton*> buttonVector;
	QComboBox* comboBox;
	QVBoxLayout* layout;

	QVector<QString> modelNameList;
	QString selectedModelName;

	void set();
};

