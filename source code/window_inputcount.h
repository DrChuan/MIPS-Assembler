//========================================================================================================================
// window_inputcount.h
//------------------------------------------------------------------------------------------------------------------------
// 此文件包括自定义输入框的类定义
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