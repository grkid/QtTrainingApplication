#pragma once
#include <qvector.h>
#include <qstring.h>
#include <QMatrix4x4>
#include <QFile>
#include <QVector>
#include <QMap>
class OpenGLSharedInfo
{
public:
    //基础的存储内容
    QVector<QString> modelPaths;
    QVector <QMatrix4x4> modelTransforms;
    QString backgroundPath = "";

    float ambientTensity = 1;
    float diffuseTensity = 1;
    float specularTensity = 1;
    float shadowIntensity = 1;
    QVector3D direction = QVector3D(-0.2, -1.0, -0.3).normalized();
    QVector3D rgbColor = QVector3D(1.0, 1.0, 1.0);

    //序列化以及反序列化
    void readSharedInfo(QString path);
    void writeSharedInfo(QString path);

    void clear();

private:
    QString matrix2str(QMatrix4x4 m);
    QMatrix4x4 str2matrix(QString s);
    QString vector2str(QVector3D v);
    QVector3D str2vector(QString s);

    const QString _strPath = "model paths";
    const QString _strMatrix = "model matrixs";
    const QString _strBack = "background";
    const QString _strAmbTensity = "ambient tensity";
    const QString _strDiffTensity = "diffuse tensity";
    const QString _strSpecTensity = "specular tensity";
    const QString _strShadowTensity = "shadow tensity";
    const QString _strDirection = "light direction";
    const QString _strColor = "light color";

    const QString _separator1 = "::";//windows目录中不会连续出现两次冒号
    const QString _separator2 = "|";
    const QString _separator3 = " ";
};