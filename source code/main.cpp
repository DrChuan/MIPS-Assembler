//========================================================================================================================
// main.cpp
//------------------------------------------------------------------------------------------------------------------------
// ������
//========================================================================================================================

#include "myAssembler.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	myAssembler w;
	w.show();
	return a.exec();
}
