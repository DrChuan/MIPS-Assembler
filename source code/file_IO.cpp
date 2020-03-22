//========================================================================================================================
// file_IO.cpp
//------------------------------------------------------------------------------------------------------------------------
// ���ļ���������������ļ����򿪺ͱ��棩�ķ���
//========================================================================================================================

#include "myAssembler.h"
#include <qfile>
#include <qtextstream.h>
#include <qdatastream.h>

void myAssembler::_open_asm()
{
	QFile *file = new QFile(filename_asm);
	// ��ֻ����ʽ��
	file->open(QFile::ReadOnly);
	// д��asm_text�ؼ�
	QTextStream in(file);
	asm_text.setText(in.readAll());
	// �ر��ļ�
	file->close();
	// �ͷ��ڴ�
	delete(file);
	// ���ñ�־
	saved_asm = 1;
}

void myAssembler::_open_bin()
{
	QFile *file = new QFile(filename_boc);
	// ��ֻ����ʽ��
	file->open(QFile::ReadOnly);
	qint32 read;
	for (int i = 0; i < MAX_INSTRUCTIONS; i++)
	{
		// ��ȡ4���ֽ�
		file->read((char*)&read, sizeof(read));
		// д�����һ��
		for (int j = 1; j < 5; j++)
		{
			// ȡ���8λ��С�˹��򣩣�ת����ʮ�������ַ���
			QString in = QString::number(read & 0xFF, 16);
			// ��ת����ֻ��һλ����ǰ�油0
			if (in.size() == 1) in = "0" + in;
			// д�뵥Ԫ��
			bin_table.item(i, j)->setText(in);
			// �Ƴ����8λ
			read >>= 8;
		}
		if (file->atEnd())
			break;
	}
	// �ر��ļ�
	file->close();
	// �ͷ��ڴ�
	delete(file);
	// ���ñ�־
	saved_boc = 1;
}

bool myAssembler::_open_coe()
{
	// ���ļ�
	QFile *file = new QFile(filename_boc);
	file->open(QFile::ReadOnly);
	// ��ȡ�ļ�����
	QString tmp = file->readAll();
	// ����
	tmp.replace('=', " = ");
	tmp.replace(';', ' ');
	tmp.replace(',', ' ');
	// �����ı���
	QTextStream stream(&tmp);
	QString get, t;
	bool ok;
	// ����Ƿ����coe�ļ���ʽ
	do { stream >> get; if (stream.atEnd()) return false;} while (get != "memory_initialization_radix");
	do { stream >> get; if (stream.atEnd()) return false;} while (get != "memory_initialization_vector");
	do { stream >> get; if (stream.atEnd()) return false;} while (get != "=");
	// д�뵥Ԫ��
	for (int i = 0; i < MAX_INSTRUCTIONS && !stream.atEnd(); i++)
	{
		// ��ȡһ��word
		stream >> get;
		// ת��Ϊ��д
		for (int j = 0; j < 4; j++)
		{
			// ��ȡ��Ӧλ���ַ�������Ƿ�Ϸ������Ϸ����������Ԫ����
			t = get.mid(j * 2, 2);
			t.toInt(&ok, 16);
			if (ok)
				bin_table.item(i, j + 1)->setText(t);
		}
	}
	return true;
}

void myAssembler::_as_asm()
{
	// �����ļ�
	QFile *file = new QFile(filename_asm);
	file->open(QFile::WriteOnly);
	// �����ַ���
	QTextStream out(file);
	// ���
    out << asm_text.toPlainText();
	// �ر��ļ�
	file->close();
	// �ͷ��ļ�ָ��
	delete(file);
}

void myAssembler::_as_bin(int count)
{
	// �����ļ�
	QFile *file = new QFile(filename_boc);
	file->open(QFile::WriteOnly);
	// �����ֽ���
	QDataStream out(file);
	int o;
	// ָ���ȡ�������ݵ����8λ��С�ι���
	unsigned char *p = (unsigned char*)&o;
	// �ӱ���ж�ȡ���ݲ�д��������
	for (int i = 0; i <= count; i++)
	{
		o = bin_table.item(i / 4, i % 4 + 1)->text().toInt((bool*)nullptr, 16);
		out << *p;
	}
	// �ر��ļ�
	file->close();
	// �ͷ��ļ�ָ��
	delete(file);
}

void myAssembler::_as_coe(int count)
{
	// �����ļ�
	QFile *file = new QFile(filename_boc);
	file->open(QFile::WriteOnly);
	// �����ַ���
	QTextStream out(file);
	// ���coeͷ����Ϣ
	out << "memory_initialization_radix=16;\nmemory_initialization_vector = \n";
	// �ӱ���ж�ȡ��Ϣ���
	for (int i = 0; i < count / 4; i++)
	{
		for (int j = 1; j <= 4; j++)
			out << bin_table.item(i, j)->text();
		out << ",\n";
	}
	for (int j = 1; j <= 4; j++)
		out << bin_table.item(count / 4, j)->text();
	out << ";";
	// �ر��ļ�
	file->close();
	// �ͷ��ļ�ָ��
	delete(file);
}