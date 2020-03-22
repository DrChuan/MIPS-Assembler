//========================================================================================================================
// slots.cpp
//------------------------------------------------------------------------------------------------------------------------
// ���ļ�������������Ĳۺ����Ķ���
//========================================================================================================================

#include "myAssembler.h"
#include "processor.h"

// �������ڴ�С
void myAssembler::resizeEvent(QResizeEvent *event)
{
	// ��������Ĵ�С
	asm_text.resize(event->size().width() - BIN_WIDTH - 70, event->size().height() - 120);
	// �������������Ĵ�С
	bin_table.move(event->size().width() - BIN_WIDTH - 25, 45);
	bin_table.resize(BIN_WIDTH, event->size().height() - 120);
	// �����ļ�������ʾ��λ��
	filename_asm_view.resize(event->size().width() - 40, 30);
	filename_asm_view.move(30, event->size().height() - 65);
	filename_boc_view.resize(event->size().width() - 40, 30);
	filename_boc_view.move(30, event->size().height() - 40);
}

// �༭�ֽ�ʱ
void myAssembler::edit_bin_begin(QTableWidgetItem *item)
{
	// �ݴ��޸�֮ǰ��ֵ
	cur_hex = item->text();
}

void myAssembler::change_asm()
{
	// ����������б仯�����ñ�־
	saved_asm = false;
}

// �޸��ֽ���ɺ�
void myAssembler::edit_bin_end(QTableWidgetItem *item)
{
	bool ok;
	// �ѱ��޸ĵĵ�ַ�е��ַ���ת����16����
	int now = item->text().toInt(&ok, 16);
	// ���ת��ʧ�ܣ���ת���������ȷ������������ʾ���ָ���ԭ����ֵ
	if (ok == false || item->text().size() != 2)
	{
		QMessageBox::critical(this, "Warning", "Please input a byte! (Between 0x00 and 0xFF)");
		item->setText(cur_hex);
	}
	// �����޸ĵ�Ԫ���ֵ��������saved_boc��־
	else
	{
		item->setText(item->text().toUpper());
		saved_boc = false;
	}
}

// �˵�����Ӧ��

void myAssembler::newfile()
{
	Inquiry check_asm, check_boc;
	// ��鲢ѯ�ʱ���asm�ļ�
	check_asm = check_save_asm();
	// �����ȡ�����������Ӧ
	if (check_asm == Cancel)
		return;
	// �����ȷ����ִ�б���
	if (check_asm == Yes)
		save_asm();
	// ��鲢ѯ�ʱ���bin/coe�ļ�
	check_boc = check_save_boc();
	// �����ȡ�����������Ӧ
	if (check_boc == Cancel)
		return;
	// �����ȷ����ִ�б���
	if (check_boc == Yes)
		save_boc();
	// ��ջ���Ͷ��������
	clear_asm_text();
	clear_bin_table();
}

void myAssembler::open_asm()
{
	// ��鲢ѯ���Ƿ񱣴�
	Inquiry check_asm = check_save_asm();
	// �����ȡ�����������Ӧ
	if (check_asm == Cancel)
		return;
	// �����ȷ��������֮
	if (check_asm == Yes)
		save_asm();
	// ִ�д򿪲���
	QString select_file = QFileDialog::getOpenFileName(this, "Open asm..", ".", "Assemble file (*.asm)");
	// ��δѡ���ļ����������Ӧ
	if (select_file == "")
		return;
	// ���ô򿪵��ļ�����������ʾ
	filename_asm = select_file;
	filename_asm_view.setText("Opened assemble file: " + filename_asm);
	// ���򿪵��ļ����ص����������
	_open_asm();
	// ���ñ�־
	have_asm = 1;
	saved_asm = 1;
}

void myAssembler::open_bin()
{
	// ��鲢ѯ���Ƿ񱣴�
	Inquiry check_boc = check_save_boc();
	// �����ȡ�����������Ӧ
	if (check_boc == Cancel)
		return;
	// �����ȷ��������֮
	if (check_boc == Yes)
		save_boc();
	// ִ�д򿪲���
	QString select_file = QFileDialog::getOpenFileName(this, "Open bin", ".", "Binary file (*.bin);;All file (*.*)");
	// ��δѡ���ļ����������Ӧ
	if (select_file == "")
		return;
	// ���ô򿪵��ļ�����������ʾ
	filename_boc = select_file;
	filename_boc_view.setText("Opened binary file: " + filename_boc);
	// ���򿪵��ļ����ص������������
	_open_bin();
	// ���ñ�־
	have_boc = 1;
	saved_boc = 1;
	boc = 1;
}

void myAssembler::open_coe()
{
	// ��鲢ѯ���Ƿ񱣴�
	Inquiry check_boc = check_save_boc();
	// �����ȡ�����������Ӧ
	if (check_boc == Cancel)
		return;
	// �����ȷ��������֮
	if (check_boc == Yes)
		save_boc();
	// ִ�д򿪲���
	QString select_file = QFileDialog::getOpenFileName(this, "Open coe", ".", "Core file (*.coe)");
	// ��δѡ���ļ����������Ӧ
	if (select_file == "")
		return;
	// ���򿪵��ļ����ص������������
	QString tmp = filename_boc;
	filename_boc = select_file;
	if (!_open_coe())
	{
		filename_boc = tmp;
		QMessageBox::critical(this, "Warning", "The format of the core file is wrong!\nPlease check the file.");
		return;
	}
	// ���ô򿪵��ļ�����������ʾ
	filename_boc_view.setText("Opened core file: " + filename_boc);
	// ���ñ�־
	have_boc = 1;
	saved_boc = 1;
	boc = 0;
}

void myAssembler::save()
{
	// ���������޸Ĺ����򱣴�֮
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
	// ��鲢ѯ���Ƿ񱣴�
	Inquiry check_boc = check_save_boc();
	// �����ȡ�����������Ӧ
	if (check_boc == Cancel)
		return;
	// �����ȷ��������֮
	if (check_boc == Yes)
		save_boc();
	// �������봰������
	clear_bin_table();
	// ��������������
	Processor *processor = new Processor(&bin_table, &asm_text, 0);
	// ִ�л��
	int result = processor->assemble();
	// �����ɹ���������������
	if (result > 0)
		QMessageBox::warning(this, "Error!", "Something went wrong in the line " + QString::number(result) + ", please check the code.");
	else if(result == 0)
		QMessageBox::warning(this, "Error!", "There are two segments have common space. Please check the segment claim!");
	// ���ɹ���������ʾ��Ϣ
	else
		QMessageBox::information(this, "Congratulations", "Successful! The result is in the right table!");
	delete(processor);
}

void myAssembler::dissemble()
{
	// ��鲢ѯ���Ƿ񱣴�
	Inquiry check_asm = check_save_asm();
	// �����ȡ�����������Ӧ
	if (check_asm == Cancel)
		return;
	// �����ȷ��������֮
	if (check_asm == Yes)
		save_asm();
	// ѯ��Ҫ�������ֽ���
	int count = _ask_count();
	if (count == -1) return;
	// ��������������
	Processor *processor = new Processor(&bin_table, &asm_text, count / 4 + 1);
	// ִ�з����
	int result = processor->dissemble();
	// ������಻�ɹ�������ʧ�ܵ�����
	if (result != -1)
		QMessageBox::warning(this, "Error!", "Something went wrong in the line " + QString::number(result) + ", please check the code.");
	// �������ɹ���������ʾ��Ϣ
	else
		QMessageBox::information(this, "Congratulations", "Successful! The result is in the left frame!");
	delete(processor);

}