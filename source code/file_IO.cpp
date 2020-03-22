//========================================================================================================================
// file_IO.cpp
//------------------------------------------------------------------------------------------------------------------------
// 此文件包括主程序操纵文件（打开和保存）的方法
//========================================================================================================================

#include "myAssembler.h"
#include <qfile>
#include <qtextstream.h>
#include <qdatastream.h>

void myAssembler::_open_asm()
{
	QFile *file = new QFile(filename_asm);
	// 以只读方式打开
	file->open(QFile::ReadOnly);
	// 写入asm_text控件
	QTextStream in(file);
	asm_text.setText(in.readAll());
	// 关闭文件
	file->close();
	// 释放内存
	delete(file);
	// 设置标志
	saved_asm = 1;
}

void myAssembler::_open_bin()
{
	QFile *file = new QFile(filename_boc);
	// 以只读方式打开
	file->open(QFile::ReadOnly);
	qint32 read;
	for (int i = 0; i < MAX_INSTRUCTIONS; i++)
	{
		// 读取4个字节
		file->read((char*)&read, sizeof(read));
		// 写入表格的一行
		for (int j = 1; j < 5; j++)
		{
			// 取最低8位（小端规则），转化成十六进制字符串
			QString in = QString::number(read & 0xFF, 16);
			// 若转化后只有一位，在前面补0
			if (in.size() == 1) in = "0" + in;
			// 写入单元格
			bin_table.item(i, j)->setText(in);
			// 移出最低8位
			read >>= 8;
		}
		if (file->atEnd())
			break;
	}
	// 关闭文件
	file->close();
	// 释放内存
	delete(file);
	// 设置标志
	saved_boc = 1;
}

bool myAssembler::_open_coe()
{
	// 打开文件
	QFile *file = new QFile(filename_boc);
	file->open(QFile::ReadOnly);
	// 读取文件内容
	QString tmp = file->readAll();
	// 处理
	tmp.replace('=', " = ");
	tmp.replace(';', ' ');
	tmp.replace(',', ' ');
	// 创建文本流
	QTextStream stream(&tmp);
	QString get, t;
	bool ok;
	// 检查是否符合coe文件格式
	do { stream >> get; if (stream.atEnd()) return false;} while (get != "memory_initialization_radix");
	do { stream >> get; if (stream.atEnd()) return false;} while (get != "memory_initialization_vector");
	do { stream >> get; if (stream.atEnd()) return false;} while (get != "=");
	// 写入单元格
	for (int i = 0; i < MAX_INSTRUCTIONS && !stream.atEnd(); i++)
	{
		// 读取一个word
		stream >> get;
		// 转化为大写
		for (int j = 0; j < 4; j++)
		{
			// 获取相应位的字符，检查是否合法，若合法则输出到单元格中
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
	// 创建文件
	QFile *file = new QFile(filename_asm);
	file->open(QFile::WriteOnly);
	// 创建字符流
	QTextStream out(file);
	// 输出
    out << asm_text.toPlainText();
	// 关闭文件
	file->close();
	// 释放文件指针
	delete(file);
}

void myAssembler::_as_bin(int count)
{
	// 创建文件
	QFile *file = new QFile(filename_boc);
	file->open(QFile::WriteOnly);
	// 创建字节流
	QDataStream out(file);
	int o;
	// 指向读取到的数据的最低8位（小段规则）
	unsigned char *p = (unsigned char*)&o;
	// 从表格中读取数据并写入数据流
	for (int i = 0; i <= count; i++)
	{
		o = bin_table.item(i / 4, i % 4 + 1)->text().toInt((bool*)nullptr, 16);
		out << *p;
	}
	// 关闭文件
	file->close();
	// 释放文件指针
	delete(file);
}

void myAssembler::_as_coe(int count)
{
	// 创建文件
	QFile *file = new QFile(filename_boc);
	file->open(QFile::WriteOnly);
	// 创建字符流
	QTextStream out(file);
	// 输出coe头部信息
	out << "memory_initialization_radix=16;\nmemory_initialization_vector = \n";
	// 从表格中读取信息输出
	for (int i = 0; i < count / 4; i++)
	{
		for (int j = 1; j <= 4; j++)
			out << bin_table.item(i, j)->text();
		out << ",\n";
	}
	for (int j = 1; j <= 4; j++)
		out << bin_table.item(count / 4, j)->text();
	out << ";";
	// 关闭文件
	file->close();
	// 释放文件指针
	delete(file);
}