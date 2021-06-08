#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include "CoreFunctionWidget.h"
#include "widget.h"
#include "MainWindow.h"
#include "BackgroundVideo.h"
int main(int argc, char* argv[])
{
	//写入x64分支信息
	QApplication a(argc, argv);

	/*Widget w;
	w.setWindowTitle("learn openGL with Qt and blalala...");
	w.show();*/

	MainWindow* w;
	w = MainWindow::getMw();
	w->setWindowTitle("Augmented Reality + Rendering");
	w->show();

	return a.exec();
}

/*
* 坐标系：
* Z轴垂直屏幕，方向朝外
* X轴水平，方向朝右
* Y轴垂直，方向朝上
*/

/*
* 光源调整策略：
* XZ平面角度，以X轴正方向为0度，顺时针增长，值域0到360，单位度，名称degXZ
* Y轴夹角，以XZ平面为0度，向Y正方向增长，反方向减小，值域-90到90，单位度，名称degY
* 从两个夹角得到方向向量：
* (cos(degXZ)*cos(degY),sin(degY),sin(degXZ)cos(degY))
*/

/*
* 阴影部分涉及到上下文引用，多次bind/release，不建议封装成单独的类。
* 阴影部分调整策略：
* TODO
*/

/*
* 拿不到深度缓冲，暂时不设置阴影。
*/

/*
* 3.19更新：拿到深度缓冲
* 如何添加阴影抗锯齿？
* 试着对阴影做一个手动的高斯低通滤波器
*/

/*
* 3.25更新：
* 修改了两个小问题：
* 1. 获取模型中心
* 2. background遮挡模型问题
* 准备加入PCSS(Percentage-Closer Soft Shadows)
*/

/*
* 平行光下阴影的两个特性：
* 阴影距离遮挡物越远，其颜色越浅
* 阴影距离遮挡物越远，其越接近于软阴影
*/