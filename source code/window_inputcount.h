//========================================================================================================================
// window_inputcount.h
//------------------------------------------------------------------------------------------------------------------------
// ���ļ������Զ����������ඨ��
//========================================================================================================================

#pragma once

#include <QtWidgets/qmessagebox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qdialogbuttonbox.h>


class Window_InputCount : public QMessageBox
{
	Q_OBJECT

public:
	Window_InputCount(QWidget *parent);
	QLineEdit lineedit;

};