#pragma once
#include <QSet>
#include <QVector3D>
#include <QEvent>
#include <QWidget>
#include <QtMath>
#include <QMatrix4x4>
#include <QKeyEvent>
#include <QTime>


class Camera
{
public:
    Camera(QWidget* widget);

    float getMoveSpeed() const;
    void setMoveSpeed(float value);

    float getSensitivity() const;
    void setSensitivity(float value);

    float getYaw() const;
    void setYaw(float value);

    float getPitch() const;
    void setPitch(float value);

    QVector3D getCameraPos() const;
    void setCameraPos(const QVector3D& value);

    void init();                    //��ʼ�������

    //void handle(QEvent* event);     //�������¼�

    QMatrix4x4 getView() const;     //��ȡ�۲����

private:
    QWidget* widget;

    float yaw;                  //ƫ����
    float pitch;                //���ӽ�
    float sensitivity;          //���������

    QVector3D cameraPos;        //�������ʼλ��

    QVector3D cameraDirection;  //���������
    QVector3D cameraRight;      //�����������
    QVector3D cameraUp;         //�����������

    float moveSpeed;    //�����ƶ��ٶ�
    QSet<int> keys;     //��¼��ǰ�����°����ļ���

    int timeId;         //��ʱ��id���˶�ʱ��������ɼ����ƶ��¼�
    float deltaTime;    // ��ǰ֡����һ֡��ʱ���
    float lastFrame;    // ��һ֡��ʱ��

    QMatrix4x4 view;    //�۲����

};