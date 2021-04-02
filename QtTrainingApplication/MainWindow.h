#pragma once
#include <qmainwindow.h>
#include <qwidget.h>
#include "MainWindow.h"
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>                      //�˵�
#include <QtWidgets/QAction>                    //�˵���
#include <QDebug>                     //���
#include <QtWidgets/QToolBar>                   // ������
#include <QtWidgets/QPushButton>                //��ť
#include <QtWidgets/QStatusBar>                 //״̬��
#include <QtWidgets/QLabel>                     //��ǩ
#include <QtWidgets/QTextEdit>                  //�ı��༭��
#include <QtWidgets/QDockWidget>                //��������
#include <QtWidgets/QDialog>                    //�Ի���
#include <QtWidgets/QMessageBox>                //��׼�Ի���
#include <QtWidgets/QFileDialog>                //�ļ��Ի���
#include <qopenglwidget.h>
#include <qstring.h>
#include <qvector.h>
#include <QHash>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include "widget.h"
#include "OpenGLSharedInfo.h"
#include "LanguageAdapter.h"
#include "ModelController.h"
#include "LightController.h"

#define WOUT(a) MainWindow::output(a)

class MainWindow :
    public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

    //����ģʽ
    static MainWindow* getMw()
    {
        //���ھ�̬�������ڲ�ʵ��
        if (mw == NULL)
            mw = new MainWindow();
        return mw;
    }

    static Widget* getOpenGLWidget()
    {
        return getMw()->openglWidget;
    }

    static void output(QString a)
    {
        getMw()->text->append(a);
    }



private:
    static MainWindow* mw;
    Widget* openglWidget=NULL;
    OpenGLSharedInfo info;

    ModelController* modelController;
    LightController* lightController;
    QTextEdit* text;

    QGridLayout* layout;
    void initLayout();

    void reloadOpenGL();
    void initMenuBar();

    //�������
    const int controllerWidth = 250;
    const int textHeight = 200;
    //void resizeEvent(QResizeEvent* event);
};

