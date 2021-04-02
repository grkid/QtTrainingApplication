#pragma once
#include <qmainwindow.h>
#include <qwidget.h>
#include "MainWindow.h"
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>                      //菜单
#include <QtWidgets/QAction>                    //菜单项
#include <QDebug>                     //输出
#include <QtWidgets/QToolBar>                   // 工具栏
#include <QtWidgets/QPushButton>                //按钮
#include <QtWidgets/QStatusBar>                 //状态栏
#include <QtWidgets/QLabel>                     //标签
#include <QtWidgets/QTextEdit>                  //文本编辑区
#include <QtWidgets/QDockWidget>                //浮动窗口
#include <QtWidgets/QDialog>                    //对话框
#include <QtWidgets/QMessageBox>                //标准对话框
#include <QtWidgets/QFileDialog>                //文件对话框
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

    //单例模式
    static MainWindow* getMw()
    {
        //类内静态函数在内部实现
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

    //布局相关
    const int controllerWidth = 250;
    const int textHeight = 200;
    //void resizeEvent(QResizeEvent* event);
};

