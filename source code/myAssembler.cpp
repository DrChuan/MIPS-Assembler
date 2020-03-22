//========================================================================================================================
// myAssembler.cpp
//------------------------------------------------------------------------------------------------------------------------
// 此文件包括主程序类的构造函数和public方法
// - check_save_asm(), check_save_boc() : 检查并询问asm和bin/coe是否需要保存
// - clear_asm_text(), clear_bin_table(): 清空汇编代码/二进制码内容
// - save_asm(), save_boc()             : 响应 新建、打开、保存时的保存动作
// - as_asm(), as_bin(), as_coe()       : 响应 保存时需要新建文件时的动作 和 另存为动作
//========================================================================================================================
#include "myAssembler.h"

myAssembler::myAssembler(QWidget *parent)
	: QMainWindow(parent),
	  asm_text(this),           bin_table(this),
	  filename_asm_view(this),  filename_boc_view(this),
	  have_asm(0), have_boc(0), saved_asm(1), saved_boc(1),
	  window_inputcount(this)
{
	// 设置初始大小
	resize(800, 600);
	// 设置最小大小
	setMinimumSize(BIN_WIDTH + 150, BIN_HEIGHT + 30);
	// 初始化各控件
	set_menu();
	init_asm_text();
	init_bin_table();
	init_filename_view();
	window_inputcount.hide();
}

myAssembler::Inquiry myAssembler::check_save_asm()
{
	QString info;
	// 如果asm框中内容有改动
	if (!saved_asm) {
		// 如果之前打开了asm文件，询问是否保存
		if (have_asm)
			info = "Save the current asm file?";
		// 如果之前没有打开asm文件，询问是否创建
		else
			info = "Save the new asm file?";
		// 弹出提示框
		QMessageBox msg(this);
		msg.setWindowTitle("myAssembler");
		msg.setText(info);
		msg.setIcon(QMessageBox::Question);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		int res = msg.exec();
		if (res == QMessageBox::Yes)
			return Yes;
		else if (res == QMessageBox::No)
			return No;
		else if (res == QMessageBox::Cancel)
			return Cancel;
	}
	else
		return No;
}

myAssembler::Inquiry myAssembler::check_save_boc()
{
	QString info;
	// 如果bin框中内容有改动
	if (!saved_boc) {
		// 如果之前打开了bin/coe文件，询问是否保存
		if (have_boc)
			info = QString("Save the current ") + ((boc == 1) ? "bin" : "coe") + " file?";
		// 如果之前没有打开bin/coe文件，询问是否创建
		else
			info = "Save the new binary code?";
		// 弹出提示框
		QMessageBox msg(this);
		msg.setWindowTitle("myAssembler");
		msg.setText(info);
		msg.setIcon(QMessageBox::Question);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		int res = msg.exec();
		if (res == QMessageBox::Yes)
			return Yes;
		else if (res == QMessageBox::No)
			return No;
		else if (res == QMessageBox::Cancel)
			return Cancel;
	}
	else
		return No;
}

void myAssembler::clear_asm_text()
{
	// 清空汇编栏的内容
	asm_text.setText("");
	saved_asm = 1;
}

void myAssembler::clear_bin_table()
{
	// 遍历每一个单元格
	for (int i = 0; i < MAX_INSTRUCTIONS; i++)
		for (int j = 0; j < 4; j++)
		{
			// 设置值
			QTableWidgetItem *it = new QTableWidgetItem("00");
			// 设置字体
			it->setFont(QFont("Monaco", 12));
			// 设置居中
			it->setTextAlignment(Qt::AlignCenter);
			delete(bin_table.item(i, j + 1));
			bin_table.setItem(i, j + 1, it);
		}
	saved_boc = 1;
}

void myAssembler::save_asm()
{
	// 如果已经打开asm，直接保存，没有提示信息
	if (have_asm)   
	{
		_as_asm();
		saved_asm = 1;
	}
	// 如果没有打开asm，询问保存位置和文件名
	else			
		as_asm();
}

void myAssembler::save_boc()
{
	// 如果已经打开bin或coe，直接保存，没有提示信息
	if (have_boc)   
	{
		int count = _ask_count();
		if (count == -1) return;
		if (boc) _as_bin(count);
		else _as_coe(count);
		saved_boc = 1;
	}
	// 如果没有打开bin或coe，询问保存方式、保存位置和文件名
	else			
	{
		// 设置对话框
		QMessageBox msg(this);
		msg.setWindowTitle("myAssembler");
		msg.setText("Save as bin or coe?");
		msg.setIcon(QMessageBox::Question);
		msg.addButton("&Bin", (QMessageBox::ButtonRole)0);
		msg.addButton("&Coe", (QMessageBox::ButtonRole)2);
		// 接收结果
		int res = msg.exec();
		// 保存为bin
		if (res == 0)
			as_bin();
		// 保存为coe
		else if (res == 1)
			as_coe();
	}
}

void myAssembler::as_asm()
{
	// 询问保存位置
	QString save_as_name = QFileDialog::getSaveFileName(this, tr("Save asm.."), "", tr("Assemble file (*.asm)"));
	// 若没有点击取消
	if (save_as_name != "")
	{
		filename_asm = save_as_name;
		filename_asm_view.setText("Opened assemble file: " + filename_asm);
		_as_asm();
		saved_asm = 1;
		have_asm = 1;
	}
}

int myAssembler::_ask_count()
{
	int yes, count;
	bool ok;
	do {
		// 清空输入框的内容
		window_inputcount.lineedit.setText("");
		// 模态调用对话框
		yes = window_inputcount.exec();
		// 若点击取消，则返回-1
		if (yes == 0x400000) return -1;
		// 转化框中内容，若不合法则继续询问，若合法则返回结果
		count = window_inputcount.lineedit.text().toInt(&ok, 16);
	} while (!ok || count < 0 || count > MAX_INSTRUCTIONS * 4);
	return count;
}

void myAssembler::as_bin()
{
	int count = _ask_count();
	if (count == -1) return;
	// 询问保存位置
	QString save_as_name = QFileDialog::getSaveFileName(this, tr("Save bin.."), "", tr("Binary file (*.bin)"));
	// 若没有点击取消
	if (save_as_name != "")
	{
		filename_boc = save_as_name;
		filename_boc_view.setText("Opened binary file: " + filename_boc);
		_as_bin(count);
		saved_boc = 1;
		have_boc = 1;
		boc = 1;
	}
}

void myAssembler::as_coe()
{
	int count = _ask_count();
	if (count == -1) return;
	// 询问保存位置
	QString save_as_name = QFileDialog::getSaveFileName(this, tr("Save coe.."), "", tr("Core file (*.coe)"));
	// 若没有点击取消
	if (save_as_name != "")
	{
		filename_boc = save_as_name;
		filename_boc_view.setText("Opened core file: " + filename_boc);
		_as_coe(count);
		saved_boc = 1;
		have_boc = 1;
		boc = 0;
	}
}