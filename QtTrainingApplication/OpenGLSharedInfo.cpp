#include "OpenGLSharedInfo.h"

void OpenGLSharedInfo::readSharedInfo(QString path)
{
	QVector<QString> lines;
	QFile file(path);
	file.open(QIODevice::ReadOnly | QIODevice::Text);

	this->clear();

	//使用QTextStream一行一行读取文件
	QTextStream in(&file);
	while (!in.atEnd())
	{
		QString line = in.readLine();
		lines.append(line);
	}
	
	//全部放进map中
	QMap<QString, QString> map;
	for (auto str : lines)
	{
		auto strs = str.split(_separator1);
		//防止background不存在
		QString strat1="";
		if (strs.size() == 2)
			strat1 = strs.at(1);
		map.insert(strs.at(0),strat1);
	}
	qDebug() << map.size();

	//处理每个字段
	QString strPath = map.find(_strPath).value();
	//试图解决中文目录问题
	strPath=strPath.toLocal8Bit();
	auto strPaths = strPath.split(_separator2);
	for (auto str : strPaths)
		this->modelPaths.append(str);

	QString strMatrix = map.find(_strMatrix).value();
	auto strMatrixs = strMatrix.split(_separator2);
	for (auto str : strMatrixs)
		this->modelTransforms.append(str2matrix(str));

	QString strBack = map.find(_strBack).value();
	strBack = strBack.toLocal8Bit();
	this->backgroundPath = strBack;

	QString strAmbTensity = map.find(_strAmbTensity).value();
	this->ambientTensity = strAmbTensity.toFloat();

	QString strDiffTensity = map.find(_strDiffTensity).value();
	this->diffuseTensity = strDiffTensity.toFloat();

	QString strSpecTensity = map.find(_strSpecTensity).value();
	this->specularTensity = strSpecTensity.toFloat();

	QString strShadowTensity = map.find(_strShadowTensity).value();
	this->shadowIntensity = strShadowTensity.toFloat();

	QString strDirection = map.find(_strDirection).value();
	this->direction = str2vector(strDirection);

	QString strColor = map.find(_strColor).value();
	this->rgbColor = str2vector(strColor);

	qDebug() << "";
}

void OpenGLSharedInfo::writeSharedInfo(QString path)
{
	//用“:”（半角）做间隔，windows下文件目录本来就没有“=”
	//序列之间用“|”做间隔，最后一个后面也有
	QFile file(path);
	bool ok = file.open(QIODevice::ReadWrite);

	QString strPath = _strPath + _separator1;
	//有无更好的处理？
	for (int i = 0; i < modelPaths.size(); i++)
	{
		if (i == 0)
		{
			strPath += modelPaths[i];
		}
		else
		{
			strPath += _separator2 + modelPaths[i];
		}
	}

	QString strMatrix = _strMatrix + _separator1;
	for (int i = 0; i < modelTransforms.size(); i++)
	{
		if (i == 0)
		{
			strMatrix += matrix2str(modelTransforms[i]);
		}
		else
		{
			strMatrix += _separator2+matrix2str(modelTransforms[i]);
		}
	}

	QString strBack = _strBack + _separator1 + backgroundPath;
	QString strAmbTensity = _strAmbTensity + _separator1 + QString::number(ambientTensity);
	QString strDiffTensity = _strDiffTensity + _separator1 + QString::number(diffuseTensity);
	QString strSpecTensity = _strSpecTensity + _separator1 + QString::number(specularTensity);
	QString strShadowTensity = _strShadowTensity + _separator1 + QString::number(shadowIntensity);
	QString strDirection = _strDirection + _separator1 + vector2str(direction);
	QString strColor = _strColor + _separator1 + vector2str(rgbColor);

	//理论上可以乱序写入
	file.write(strPath.toUtf8()+"\n");
	file.write(strMatrix.toUtf8() + "\n");
	file.write(strBack.toUtf8() + "\n");
	file.write(strAmbTensity.toUtf8() + "\n");
	file.write(strDiffTensity.toUtf8() + "\n");
	file.write(strSpecTensity.toUtf8() + "\n");
	file.write(strShadowTensity.toUtf8() + "\n");
	file.write(strDirection.toUtf8() + "\n");
	file.write(strColor.toUtf8() + "\n");

	file.close();
}

void OpenGLSharedInfo::clear()
{
	modelPaths.clear();
	modelTransforms.clear();
	backgroundPath = "";
	ambientTensity = 1;
	diffuseTensity = 1;
	specularTensity = 1;
	shadowIntensity = 1;
	direction = QVector3D(-0.2, -1.0, -0.3).normalized();
	rgbColor = QVector3D(1.0, 1.0, 1.0);
}

QString OpenGLSharedInfo::matrix2str(QMatrix4x4 m)
{
	QString s = "";
	for (int i = 0; i < 4 * 4; i++)
	{
		if (i == 0)
			s += QString::number(m.data()[i]);
		else
		{
			s += _separator3;
			s += QString::number(m.data()[i]);
		}
	}
	return s;
}

QMatrix4x4 OpenGLSharedInfo::str2matrix(QString s)
{
	QMatrix4x4 m;
	auto strs = s.split(_separator3);
	for (int i = 0; i < 4 * 4; i++)
	{
		m.data()[i] = strs.at(i).toFloat();
	}
	return m;
	//性能问题？
}

QString OpenGLSharedInfo::vector2str(QVector3D v)
{
	QString s = QString::number(v.x()) + _separator3 + QString::number(v.y()) + _separator3 + QString::number(v.z());
	return s;
}

QVector3D OpenGLSharedInfo::str2vector(QString s)
{
	QVector3D v;
	auto strs = s.split(_separator3);
	v.setX(strs.at(0).toFloat());
	v.setY(strs.at(1).toFloat());
	v.setZ(strs.at(2).toFloat());
	return v;
}
