//========================================================================================================================
// window_inputcount.cpp
//------------------------------------------------------------------------------------------------------------------------
// ���ļ������Զ��������Ĺ��캯������
//========================================================================================================================

#include "window_inputcount.h"

Window_InputCount::Window_InputCount(QWidget *parent) : QMessageBox(parent), lineedit(this)
{
	setText("Please input the address of the   \nlast byte you expect:\n\n\n");
	setFixedSize(350, 300);
	lineedit.resize(120, 35);
	lineedit.move(80, 70);
	setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
}