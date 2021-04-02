
#include "camera.h"

Camera::Camera(QWidget* widget)
    : widget(widget)
    , yaw(0)
    , pitch(0)
    , sensitivity(0.005f)
    , cameraPos(0.0f, 0.0f, 5.0f)
    , cameraDirection(0,0,-1)
    , cameraRight(QVector3D::crossProduct({ 0.0f,1.0f,0.0f }, cameraDirection))
    , cameraUp(QVector3D::crossProduct(cameraDirection, cameraRight))
    , moveSpeed(0.5f)
    , timeId(0)
{
}

float Camera::getMoveSpeed() const
{
    return moveSpeed;
}

void Camera::setMoveSpeed(float value)
{
    moveSpeed = value;
}

float Camera::getSensitivity() const
{
    return sensitivity;
}

void Camera::setSensitivity(float value)
{
    sensitivity = value;
}

float Camera::getYaw() const
{
    return yaw;
}

void Camera::setYaw(float value)
{
    yaw = value;
}

float Camera::getPitch() const
{
    return pitch;
}

void Camera::setPitch(float value)
{
    pitch = value;
}

QVector3D Camera::getCameraPos() const
{
    return cameraPos;
}

void Camera::setCameraPos(const QVector3D& value)
{
    cameraPos = value;
}

//void Camera::handle(QEvent* e)
//{
//    if (e->type() == QEvent::MouseMove) {
//        QMouseEvent* event = static_cast<QMouseEvent*>(e);
//        float xoffset = event->x() - widget->rect().center().x();
//        float yoffset = widget->rect().center().y() - event->y(); // ע���������෴�ģ���Ϊy�����Ǵӵײ����������������
//        xoffset *= sensitivity;
//        yoffset *= sensitivity;
//        yaw += xoffset;
//        pitch += yoffset;
//        if (pitch > 1.55)         //�����ӽ����Ƶ�[-89��,89��]��89��Լ����1.55
//            pitch = 1.55;
//        if (pitch < -1.55)
//            pitch = -1.55;
//        cameraDirection.setX(cos(yaw) * cos(pitch));
//        cameraDirection.setY(sin(pitch));
//        cameraDirection.setZ(sin(yaw) * cos(pitch));
//        view.setToIdentity();
//        view.lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
//        QCursor::setPos(widget->geometry().center());       //������ƶ���������
//    }
//    else if (e->type() == QEvent::Timer) {
//        float cameraSpeed = moveSpeed * deltaTime;
//        // �ƶ�ģ�ͣ�����ȫ���෴
//        if (keys.contains(Qt::Key_W))                           //ǰ
//            cameraPos += cameraSpeed * cameraDirection;
//        if (keys.contains(Qt::Key_S))                           //��
//            cameraPos -= cameraSpeed * cameraDirection;
//        if (keys.contains(Qt::Key_A))                           //��
//            cameraPos -= QVector3D::crossProduct(cameraDirection, cameraUp) * cameraSpeed;
//        if (keys.contains(Qt::Key_D))                           //��
//            cameraPos += QVector3D::crossProduct(cameraDirection, cameraUp) * cameraSpeed;
//        if (keys.contains(Qt::Key_Space))                       //�ϸ�
//            cameraPos.setY(cameraPos.y() + cameraSpeed);
//        if (keys.contains(Qt::Key_Shift))                       //�³�
//            cameraPos.setY(cameraPos.y() - cameraSpeed);
//
//        view.setToIdentity();
//        view.lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
//    }
//    else if (e->type() == QEvent::KeyPress) {
//        //isAutoRepeat�����жϴ˰�������Դ�Ƿ��ǳ���
//        QKeyEvent* event = static_cast<QKeyEvent*>(e);
//        keys.insert(event->key());                              //��Ӱ���
//        if (!event->isAutoRepeat() && timeId == 0) {                  //�����ʱ��δ��������������ʱ��
//            timeId = widget->startTimer(1);
//        }
//    }
//    else if (e->type() == QEvent::KeyRelease) {
//        QKeyEvent* event = static_cast<QKeyEvent*>(e);
//        keys.remove(event->key());
//        if (!event->isAutoRepeat() && timeId != 0 && keys.empty()) {    //��û�а��������Ҷ�ʱ���������У��Źرն�ʱ��
//            widget->killTimer(timeId);
//            timeId = 0;                                          //���ö�ʱ��id
//        }
//    }
//    else if (e->type() == QEvent::UpdateRequest) {
//        float time = QTime::currentTime().msecsSinceStartOfDay() / 1000.0;
//        deltaTime = time - lastFrame;                           //�ڴ˴�����ʱ���
//        lastFrame = time;
//    }
//    else if (e->type() == QEvent::FocusIn) {
//        widget->setCursor(Qt::BlankCursor);             //���������
//        QCursor::setPos(widget->geometry().center());   //������ƶ���������
//        widget->setMouseTracking(true);                 //�������׷��
//    }
//    else if (e->type() == QEvent::FocusOut) {
//        widget->setCursor(Qt::ArrowCursor);   //�ָ������
//        widget->setMouseTracking(false);      //�ر����׷��
//    }
//}

void Camera::init()
{
    view.lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
    //widget->activateWindow();                 //�����
    //widget->setFocus();

}

QMatrix4x4 Camera::getView() const
{
    return view;
}