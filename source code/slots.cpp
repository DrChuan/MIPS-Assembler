//========================================================================================================================
// slots.cpp
//------------------------------------------------------------------------------------------------------------------------
// 此文件包括主程序类的槽函数的定义
//========================================================================================================================

#include "myAssembler.h"
#include "processor.h"

// 调整窗口大小
void myAssembler::resizeEvent(QResizeEvent *event)
{
	// 调整汇编框的大小
	asm_text.resize(event->size().width() - BIN_WIDTH - 70, event->size().height() - 120);
	// 调整二进制码表的大小
	bin_table.move(event->size().width() - BIN_WIDTH - 25, 45);
	bin_table.resize(BIN_WIDTH, event->size().height() - 120);
	// 调整文件名字显示的位置
	filename_asm_view.resize(event->size().width() - 40, 30);
	filename_asm_view.move(30, event->size().height() - 65);
	filename_boc_view.resize(event->size().width() - 40, 30);
	filename_boc_view.move(30, event->size().height() - 40);
}

// 编辑字节时
void myAssembler::edit_bin_begin(QTableWidgetItem *item)
{
	// 暂存修改之前的值
	cur_hex = item->text();
}

void myAssembler::change_asm()
{
	// 如果汇编框中有变化，设置标志
	saved_asm = false;
}

// 修改字节完成后
void myAssembler::edit_bin_end(QTableWidgetItem *item)
{
	bool ok;
	// 把被修改的地址中的字符串转化成16进制
	int now = item->text().toInt(&ok, 16);
	// 如果转化失败，或转化结果不正确，给出错误提示，恢复到原来的值
	if (ok == false || item->text().size() != 2)
	{
		QMessageBox::critical(this, "Warning", "Please input a byte! (Between 0x00 and 0xFF)");
		item->setText(cur_hex);
	}
	// 否则修改单元格的值，并设置saved_boc标志
	else
	{
		item->setText(item->text().toUpper());
		saved_boc = false;
	}
}

// 菜单栏响应槽

void myAssembler::newfile()
{
	Inquiry check_asm, check_boc;
	// 检查并询问保存asm文件
	check_asm = check_save_asm();
	// 若点击取消，则结束响应
	if (check_asm == Cancel)
		return;
	// 若点击确定，执行保存
	if (check_asm == Yes)
		save_asm();
	// 检查并询问保存bin/coe文件
	check_boc = check_save_boc();
	// 若点击取消，则结束相应
	if (check_boc == Cancel)
		return;
	// 若点击确定，执行保存
	if (check_boc == Yes)
		save_boc();
	// 清空汇编框和二进制码表
	clear_asm_text();
	clear_bin_table();
}

void myAssembler::open_asm()
{
	// 检查并询问是否保存
	Inquiry check_asm = check_save_asm();
	// 若点击取消，则结束响应
	if (check_asm == Cancel)
		return;
	// 若点击确定，保存之
	if (check_asm == Yes)
		save_asm();
	// 执行打开操作
	QString select_file = QFileDialog::getOpenFileName(this, "Open asm..", ".", "Assemble file (*.asm)");
	// 若未选择文件，则结束响应
	if (select_file == "")
		return;
	// 设置打开的文件名并更新显示
	filename_asm = select_file;
	filename_asm_view.setText("Opened assemble file: " + filename_asm);
	// 将打开的文件加载到汇编代码框中
	_open_asm();
	// 设置标志
	have_asm = 1;
	saved_asm = 1;
}

void myAssembler::open_bin()
{
	// 检查并询问是否保存
	Inquiry check_boc = check_save_boc();
	// 若点击取消，则结束响应
	if (check_boc == Cancel)
		return;
	// 若点击确定，保存之
	if (check_boc == Yes)
		save_boc();
	// 执行打开操作
	QString select_file = QFileDialog::getOpenFileName(this, "Open bin", ".", "Binary file (*.bin);;All file (*.*)");
	// 若未选择文件，则结束响应
	if (select_file == "")
		return;
	// 设置打开的文件名并更新显示
	filename_boc = select_file;
	filename_boc_view.setText("Opened binary file: " + filename_boc);
	// 将打开的文件加载到二进制码表中
	_open_bin();
	// 设置标志
	have_boc = 1;
	saved_boc = 1;
	boc = 1;
}

void myAssembler::open_coe()
{
	// 检查并询问是否保存
	Inquiry check_boc = check_save_boc();
	// 若点击取消，则结束响应
	if (check_boc == Cancel)
		return;
	// 若点击确定，保存之
	if (check_boc == Yes)
		save_boc();
	// 执行打开操作
	QString select_file = QFileDialog::getOpenFileName(this, "Open coe", ".", "Core file (*.coe)");
	// 若未选择文件，则结束响应
	if (select_file == "")
		return;
	// 将打开的文件加载到二进制码表中
	QString tmp = filename_boc;
	filename_boc = select_file;
	if (!_open_coe())
	{
		filename_boc = tmp;
		QMessageBox::critical(this, "Warning", "The format of the core file is wrong!\nPlease check the file.");
		return;
	}
	// 设置打开的文件名并更新显示
	filename_boc_view.setText("Opened core file: " + filename_boc);
	// 设置标志
	have_boc = 1;
	saved_boc = 1;
	boc = 0;
}

void myAssembler::save()
{
	// 若汇编代码修改过，则保存之
	if(saved_asm == 0)
		save_asm();
	save_boc();
}

void myAssembler::save_as_asm()
{
	as_asm();
}

void myAssembler::save_as_bin()
{
	as_bin();
}

void myAssembler::save_as_coe()
{
	as_coe();
}

void myAssembler::exit()
{
	this->close();
}

void myAssembler::assemble()
{
	// 检查并询问是否保存
	Inquiry check_boc = check_save_boc();
	// 若点击取消，则结束响应
	if (check_boc == Cancel)
		return;
	// 若点击确定，保存之
	if (check_boc == Yes)
		save_boc();
	// 二进制码窗口清零
	clear_bin_table();
	// 创建处理器对象
	Processor *processor = new Processor(&bin_table, &asm_text, 0);
	// 执行汇编
	int result = processor->assemble();
	// 若不成功，报告出错的行数
	if (result > 0)
		QMessageBox::warning(this, "Error!", "Something went wrong in the line " + QString::number(result) + ", please check the code.");
	else if(result == 0)
		QMessageBox::warning(this, "Error!", "There are two segments have common space. Please check the segment claim!");
	// 若成功，给出提示信息
	else
		QMessageBox::information(this, "Congratulations", "Successful! The result is in the right table!");
	delete(processor);
}

void myAssembler::dissemble()
{
	// 检查并询问是否保存
	Inquiry check_asm = check_save_asm();
	// 若点击取消，则结束响应
	if (check_asm == Cancel)
		return;
	// 若点击确定，保存之
	if (check_asm == Yes)
		save_asm();
	// 询问要反汇编的字节数
	int count = _ask_count();
	if (count == -1) return;
	// 创建处理器对象
	Processor *processor = new Processor(&bin_table, &asm_text, count / 4 + 1);
	// 执行反汇编
	int result = processor->dissemble();
	// 若反汇编不成功，报告失败的行数
	if (result != -1)
		QMessageBox::warning(this, "Error!", "Something went wrong in the line " + QString::number(result) + ", please check the code.");
	// 若反汇编成功，给出提示信息
	else
		QMessageBox::information(this, "Congratulations", "Successful! The result is in the left frame!");
	delete(processor);

}