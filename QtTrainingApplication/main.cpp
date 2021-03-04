#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include "CoreFunctionWidget.h"
#include "widget.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	Widget w;
	w.setWindowTitle("learn openGL with Qt and blalala...");
	w.show();

	return a.exec();
}