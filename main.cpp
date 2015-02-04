#include "stdafx.h"
#include "imagepacker.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ImagePacker w;
	w.show();
	return a.exec();
}
