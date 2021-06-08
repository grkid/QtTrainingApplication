#include "MainWindow.h"

MainWindow* MainWindow::mw = NULL;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    this->resize(QSize(1920, 1080));
    this->initMenuBar();

    initLayout();
    reloadOpenGL();
}

void MainWindow::initLayout()
{
    modelController = new ModelController(this);
    lightController = new LightController(this);

    text = new QTextEdit(this);
    /*text->setText("debug info\n");*/
    text->setMaximumHeight(300);
    text->setMinimumHeight(20);
    //text->setEnabled(false);
    //text->setFocusPolicy(Qt::NoFocus);
    text->setReadOnly(true);

    layout = new QGridLayout();
    layout->addWidget(modelController,0,0,0,1);
    layout->addWidget(lightController,0,1,0,1);
    layout->addWidget(openglWidget,0,2);
    layout->addWidget(text, 1,2);
    setCentralWidget(new QWidget(this));
    centralWidget()->setLayout(layout);
}

MainWindow::~MainWindow()
{
    if (openglWidget)
        delete openglWidget;
    if (modelController)
        delete modelController;
    if (lightController)
        delete lightController;
}

void MainWindow::initMenuBar()
{
    //菜单栏
    QMenuBar* mBar = new QMenuBar(this);
    //添加菜单
    QMenu* fileMenu = mBar->addMenu(TR("文件"));
    fileMenu->addSeparator();
    QAction* readBackground = fileMenu->addAction(TR("读取背景"));
    fileMenu->addSeparator();
    QAction* readModel = fileMenu->addAction(TR("读取模型"));
    fileMenu->addSeparator();
    QAction* readPrefabricatedModel = fileMenu->addAction(TR("读取预制模型（TODO）"));
    fileMenu->addSeparator();
    QAction* saveImage = fileMenu->addAction(TR("保存"));

    connect(readBackground, &QAction::triggered,
        [=]()
        {
            QString path = QFileDialog::getOpenFileName(
                this,
                TR("读取背景"),
                "../",
                "Files(*)"
            );
            if (path != "") {
                info.backgroundPath = path;
                openglWidget->gatherInfo(info);
                reloadOpenGL();
            }
        }
    );

    connect(readModel, &QAction::triggered,
        [=]()
        {
            QString path = QFileDialog::getOpenFileName(
                this,
                TR("读取模型"),
                "../",
                "Files(*)"
            );
            if (path != "") {
                openglWidget->gatherInfo(info);
                info.modelPaths.append(path);
                info.modelTransforms.append(QMatrix4x4());
                modelController->addmodelName(QFileInfo(path).baseName());
                reloadOpenGL();
            }
        }
    );

    connect(saveImage, &QAction::triggered,
        [=]()
        {
            QString path = QFileDialog::getSaveFileName(
            this,
            TR("保存文件"),
            "../",
            "Files(*)"
            );

            if (path != "") {
                openglWidget->saveResult(path);
            }
        }
    );

    //导入按钮 TODO 之后再完成
    QMenu* importMenu = mBar->addMenu(TR("导入"));

    QMenu* sceneMenu = mBar->addMenu(TR("场景"));

    QAction* loadScene = sceneMenu->addAction(TR("读取场景"));
    sceneMenu->addSeparator();
    QAction* saveScene = sceneMenu->addAction(TR("写入场景"));

    connect(loadScene, &QAction::triggered,
        [=]()
        {
            QString path = QFileDialog::getOpenFileName(
                this,
                TR("读取场景"),
                "../",
                "Files(*)"
            );
            if (path != "") {
                info.readSharedInfo(path);
                reloadOpenGL();
                //同步各个组件
                modelController->clearModelName();
                for(auto item:info.modelPaths)
                    modelController->addmodelName(QFileInfo(item).baseName());
                lightController->sync(info);
            }
        }
    );

    connect(saveScene, &QAction::triggered,
        [=]()
        {
            QString path = QFileDialog::getSaveFileName(
                this,
                TR("写入场景"),
                "../",
                "Files(*)"
            );
            if (path != "") {
                openglWidget->gatherInfo(info);//同步
                info.writeSharedInfo(path);
                //不需要做其他的操作
            }
        }
    );



    this->setMenuBar(mBar);
}

//void MainWindow::resizeEvent(QResizeEvent* event)
//{
//    //event can be nullptr
//    int width, height;
//    width = this->width();
//    height = this->height();
//    resize(width, height);
//    modelController->resize(controllerWidth, height);
//    lightController->resize(controllerWidth, height);
//    text->resize(width, textHeight);
//    QSize openglSize = openglWidget->getSize();
//    if (openglSize == QSize(-1, -1))
//        openglWidget->resize(width - controllerWidth, height - textHeight);
//    else
//    {
//        int maxWidth = width - controllerWidth;
//        int maxHeight = height - textHeight;
//        double ratio = (double)openglSize.width() / (double)openglSize.height();
//        double currentRatio = (double)maxWidth / (double)maxHeight;
//        if (currentRatio < ratio)
//        {
//            maxHeight = maxWidth / ratio;
//        }
//        else //current ratio >= ratio
//        {
//            maxWidth = maxHeight * ratio;
//        }
//        openglWidget->resize(maxWidth, maxHeight);
//    }
//
//}

void MainWindow::reloadOpenGL()
{
    Widget* old = openglWidget;
    if (old)
    {
        old->setParent(NULL);
        layout->removeWidget(old);
        //old->gatherInfo(info);
    }
    openglWidget = new Widget(info);
    layout->addWidget(openglWidget, 0, 2);
    if(old)
        delete old;
    resizeEvent(nullptr);
}