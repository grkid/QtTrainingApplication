#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include "CoreFunctionWidget.h"
#include "widget.h"
#include "MainWindow.h"
#include "BackgroundVideo.h"
int main(int argc, char* argv[])
{
	//д��x64��֧��Ϣ
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
* ����ϵ��
* Z�ᴹֱ��Ļ��������
* X��ˮƽ��������
* Y�ᴹֱ��������
*/

/*
* ��Դ�������ԣ�
* XZƽ��Ƕȣ���X��������Ϊ0�ȣ�˳ʱ��������ֵ��0��360����λ�ȣ�����degXZ
* Y��нǣ���XZƽ��Ϊ0�ȣ���Y�������������������С��ֵ��-90��90����λ�ȣ�����degY
* �������нǵõ�����������
* (cos(degXZ)*cos(degY),sin(degY),sin(degXZ)cos(degY))
*/

/*
* ��Ӱ�����漰�����������ã����bind/release���������װ�ɵ������ࡣ
* ��Ӱ���ֵ������ԣ�
* TODO
*/

/*
* �ò�����Ȼ��壬��ʱ��������Ӱ��
*/

/*
* 3.19���£��õ���Ȼ���
* ��������Ӱ����ݣ�
* ���Ŷ���Ӱ��һ���ֶ��ĸ�˹��ͨ�˲���
*/

/*
* 3.25���£�
* �޸�������С���⣺
* 1. ��ȡģ������
* 2. background�ڵ�ģ������
* ׼������PCSS(Percentage-Closer Soft Shadows)
*/

/*
* ƽ�й�����Ӱ���������ԣ�
* ��Ӱ�����ڵ���ԽԶ������ɫԽǳ
* ��Ӱ�����ڵ���ԽԶ����Խ�ӽ�������Ӱ
*/