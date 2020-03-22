//========================================================================================================================
// myAssembler.cpp
//------------------------------------------------------------------------------------------------------------------------
// ���ļ�������������Ĺ��캯����public����
// - check_save_asm(), check_save_boc() : ��鲢ѯ��asm��bin/coe�Ƿ���Ҫ����
// - clear_asm_text(), clear_bin_table(): ��ջ�����/������������
// - save_asm(), save_boc()             : ��Ӧ �½����򿪡�����ʱ�ı��涯��
// - as_asm(), as_bin(), as_coe()       : ��Ӧ ����ʱ��Ҫ�½��ļ�ʱ�Ķ��� �� ���Ϊ����
//========================================================================================================================
#include "myAssembler.h"

myAssembler::myAssembler(QWidget *parent)
	: QMainWindow(parent),
	  asm_text(this),           bin_table(this),
	  filename_asm_view(this),  filename_boc_view(this),
	  have_asm(0), have_boc(0), saved_asm(1), saved_boc(1),
	  window_inputcount(this)
{
	// ���ó�ʼ��С
	resize(800, 600);
	// ������С��С
	setMinimumSize(BIN_WIDTH + 150, BIN_HEIGHT + 30);
	// ��ʼ�����ؼ�
	set_menu();
	init_asm_text();
	init_bin_table();
	init_filename_view();
	window_inputcount.hide();
}

myAssembler::Inquiry myAssembler::check_save_asm()
{
	QString info;
	// ���asm���������иĶ�
	if (!saved_asm) {
		// ���֮ǰ����asm�ļ���ѯ���Ƿ񱣴�
		if (have_asm)
			info = "Save the current asm file?";
		// ���֮ǰû�д�asm�ļ���ѯ���Ƿ񴴽�
		else
			info = "Save the new asm file?";
		// ������ʾ��
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
	// ���bin���������иĶ�
	if (!saved_boc) {
		// ���֮ǰ����bin/coe�ļ���ѯ���Ƿ񱣴�
		if (have_boc)
			info = QString("Save the current ") + ((boc == 1) ? "bin" : "coe") + " file?";
		// ���֮ǰû�д�bin/coe�ļ���ѯ���Ƿ񴴽�
		else
			info = "Save the new binary code?";
		// ������ʾ��
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
	// ��ջ����������
	asm_text.setText("");
	saved_asm = 1;
}

void myAssembler::clear_bin_table()
{
	// ����ÿһ����Ԫ��
	for (int i = 0; i < MAX_INSTRUCTIONS; i++)
		for (int j = 0; j < 4; j++)
		{
			// ����ֵ
			QTableWidgetItem *it = new QTableWidgetItem("00");
			// ��������
			it->setFont(QFont("Monaco", 12));
			// ���þ���
			it->setTextAlignment(Qt::AlignCenter);
			delete(bin_table.item(i, j + 1));
			bin_table.setItem(i, j + 1, it);
		}
	saved_boc = 1;
}

void myAssembler::save_asm()
{
	// ����Ѿ���asm��ֱ�ӱ��棬û����ʾ��Ϣ
	if (have_asm)   
	{
		_as_asm();
		saved_asm = 1;
	}
	// ���û�д�asm��ѯ�ʱ���λ�ú��ļ���
	else			
		as_asm();
}

void myAssembler::save_boc()
{
	// ����Ѿ���bin��coe��ֱ�ӱ��棬û����ʾ��Ϣ
	if (have_boc)   
	{
		int count = _ask_count();
		if (count == -1) return;
		if (boc) _as_bin(count);
		else _as_coe(count);
		saved_boc = 1;
	}
	// ���û�д�bin��coe��ѯ�ʱ��淽ʽ������λ�ú��ļ���
	else			
	{
		// ���öԻ���
		QMessageBox msg(this);
		msg.setWindowTitle("myAssembler");
		msg.setText("Save as bin or coe?");
		msg.setIcon(QMessageBox::Question);
		msg.addButton("&Bin", (QMessageBox::ButtonRole)0);
		msg.addButton("&Coe", (QMessageBox::ButtonRole)2);
		// ���ս��
		int res = msg.exec();
		// ����Ϊbin
		if (res == 0)
			as_bin();
		// ����Ϊcoe
		else if (res == 1)
			as_coe();
	}
}

void myAssembler::as_asm()
{
	// ѯ�ʱ���λ��
	QString save_as_name = QFileDialog::getSaveFileName(this, tr("Save asm.."), "", tr("Assemble file (*.asm)"));
	// ��û�е��ȡ��
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
		// �������������
		window_inputcount.lineedit.setText("");
		// ģ̬���öԻ���
		yes = window_inputcount.exec();
		// �����ȡ�����򷵻�-1
		if (yes == 0x400000) return -1;
		// ת���������ݣ������Ϸ������ѯ�ʣ����Ϸ��򷵻ؽ��
		count = window_inputcount.lineedit.text().toInt(&ok, 16);
	} while (!ok || count < 0 || count > MAX_INSTRUCTIONS * 4);
	return count;
}

void myAssembler::as_bin()
{
	int count = _ask_count();
	if (count == -1) return;
	// ѯ�ʱ���λ��
	QString save_as_name = QFileDialog::getSaveFileName(this, tr("Save bin.."), "", tr("Binary file (*.bin)"));
	// ��û�е��ȡ��
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
	// ѯ�ʱ���λ��
	QString save_as_name = QFileDialog::getSaveFileName(this, tr("Save coe.."), "", tr("Core file (*.coe)"));
	// ��û�е��ȡ��
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