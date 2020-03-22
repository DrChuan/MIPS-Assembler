//========================================================================================================================
// myAssembler.h
//------------------------------------------------------------------------------------------------------------------------
// 此文件包括主程序类的定义
//========================================================================================================================

#pragma once
#include <QtWidgets/QMainWindow>
#include <qstring.h>
#include <qmenu.h>
#include <qmenubar.h>
#include <qaction.h>
#include <qtextedit.h>
#include <QResizeEvent>
#include <qtablewidget.h>
#include <qheaderview.h>
#include <sstream>
#include <iomanip>
#include <qregexp.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qlabel.h>
#include <qtextstream.h>
#include "asm_text_highlight.h"
#include "regs_and_oprtrs.h"
#include "window_inputcount.h"


class myAssembler : public QMainWindow
{
	Q_OBJECT

protected:
	virtual void resizeEvent(QResizeEvent *event) override;

private:
	// 常数定义
	const int BIN_WIDTH = 350;
	const int BIN_HEIGHT = 200;
	// 窗口控件
	QTextEdit    asm_text;
	QTableWidget bin_table;
	Window_InputCount window_inputcount;
	QLabel       filename_asm_view;
	QLabel       filename_boc_view;
	// 文件
	QString filename_asm;
	QString filename_boc;
	int     have_asm;
	int     have_boc;     // 0表示没有打开，1表示打开bin，2表示打开coe
	int     boc;
	bool    saved_asm;
	bool    saved_boc;
	// 特殊用途
	QString cur_hex;  // 用于检测用户对右侧二进制码的修改是否合法
	// 初始化
	void set_menu();
	void init_asm_text();
	void init_bin_table();
	void init_filename_view();
	// 供public方法调用
	void _open_asm();
	void _open_bin();
	bool _open_coe();
	void _as_asm();
	void _as_bin(int count);
	void _as_coe(int count);
	int  _ask_count();

public:
	myAssembler(QWidget *parent = Q_NULLPTR);
	enum Inquiry { Cancel = -1, No = 0, Yes = 1 };
	void clear_bin_table();
	void clear_asm_text();
	Inquiry check_save_asm();
	Inquiry check_save_boc();
	void save_asm();
	void save_boc();
	void as_asm();
	void as_bin();
	void as_coe();

public slots:
	void newfile();
	void open_asm();
	void open_bin();
	void open_coe();
	void save();
	void save_as_asm();
	void save_as_bin();
	void save_as_coe();
	void exit();
	void change_asm();
	void edit_bin_begin(QTableWidgetItem *item);
	void edit_bin_end(QTableWidgetItem *item);
	void assemble();
	void dissemble();
};
