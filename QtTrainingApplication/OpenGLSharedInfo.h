#pragma once
#include <qvector.h>
#include <qstring.h>
#include <QMatrix4x4>
struct OpenGLSharedInfo
{
    QVector<QString> modelPaths;
    QVector <QMatrix4x4> modelTransforms;
    QString backgroundPath = "";

    //TODO:¸÷ÖÖ¾ØÕó
};