//========================================================================================================================
// asm_to_bin.cpp
//------------------------------------------------------------------------------------------------------------------------
// 数据段和代码段分别进行汇编的两个方法
//========================================================================================================================

#include "processor.h"

bool Segment::_data_to_binary(QTableWidget *bin_table, QString &code, int addr, int data_type)
{
	bool ok;
	unsigned int t;
	QString out;
	// 字节定义
	if (data_type == 1)
	{
		// 移除db标志和空格
		code.remove("db", Qt::CaseInsensitive);
		code.remove(' ');
		// 按逗号分隔字节
		QStringList bytes = code.split(',');
		// 遍历每个字节
		for (int i = 0; i < bytes.size(); i++)
		{
			QString byte = bytes[i];
			// 10进制数
			t = byte.toInt(&ok);
			if (ok) {
				if (t > 255 || t < 0) return false;
				// 若是合法的十进制数，将其转为字符串
				out = QString::number(t, 16);
				if (out.size() == 1) out = "0" + out;
			}
			// 16进制数
			else if (byte.startsWith("0x"))
			{
				byte.remove("0x");
				t = byte.toInt(&ok, 16);
				if (ok) {
					if (t > 255 || t < 0) return false;
					// 若是合法的十六进制数，将其转为字符串
					out = QString::number(t, 16);
					if (out.size() == 1) out = "0" + out;
				}
				else return false;
			}
			// 字符
			else if (byte.startsWith('\'') && byte.endsWith('\''))
			{
				// 取出单引号中间的字符
				char ch = byte.data()[1].toLatin1();
				// 将其ASCII码放入输出字符串
				out = QString::number((int)ch, 16);
				if (out.size() == 1) out = "0" + out;
			}
			else return false;
			// 将输出字符串写入表格
			bin_table->item((addr + i) / 4, (addr + i) % 4 + 1)->setText(out);
		}
	}
	// word定义
	else if (data_type == 2)
	{
		code.remove("dw", Qt::CaseInsensitive);
		code.remove(' ');
		QStringList words = code.split(',');
		for (int i = 0; i < words.size(); i++)
		{
			QString word = words[i];
			// 10进制数
			t = word.toInt(&ok);
			if (ok) {
				if (t > 65535 || t < 0) return false;
				out = QString::number(t, 16);
				out = QString(4 - out.size(), '0') + out;
			}
			// 16进制数
			else if (word.startsWith("0x"))
			{
				word.remove("0x");
				t = word.toInt(&ok, 16);
				if (ok) {
					if (t > 65535 || t < 0) return false;
					out = QString::number(t, 16);
					out = QString(4 - out.size(), '0') + out;
				}
				else return false;
			}
			// 字符
			else if (word.startsWith('\'') && word.endsWith('\''))
			{
				char ch1 = word.data()[1].toLatin1();
				char ch2 = word.data()[2].toLatin1();
				out = QString::number((int)ch1, 16);
				QString temp = QString::number((int)ch2, 16);
				if (out.size() == 1) out = "0" + out;
				if (out == "60") out = "00";
				if (temp.size() == 1) temp = "0" + temp;
				if (temp == "60") temp = "00";
				out = temp + out;
			}
			else return false;
			bin_table->item((addr + i * 2) / 4, (addr + i * 2) % 4 + 2)->setText(out.left(2));
			bin_table->item((addr + i * 2) / 4, (addr + i * 2) % 4 + 1)->setText(out.right(2));
		}
	}
	// double word定义
	else if (data_type == 4)
	{
		code.remove("dd", Qt::CaseInsensitive);
		code.remove(' ');
		QStringList dwords = code.split(',');
		for (int i = 0; i < dwords.size(); i++)
		{
			QString dword = dwords[i];
			// 10进制数
			t = dword.toInt(&ok);
			if (ok) {
				out = QString::number(t, 16);
				out = QString(8 - out.size(), '0') + out;
			}
			// 16进制数
			else if (dword.startsWith("0x"))
			{
				dword.remove("0x");
				t = dword.toUInt(&ok, 16);
				if (ok) {
					out = QString::number(t, 16);
					out = QString(8 - out.size(), '0') + out;
				}
				else return false;
			}
			// 字符
			else if (dword.startsWith('\'') && dword.endsWith('\''))
			{
				char ch1 = dword.data()[1].toLatin1();
				char ch2 = dword.data()[2].toLatin1();
				char ch3 = dword.data()[3].toLatin1();
				char ch4 = dword.data()[4].toLatin1();
				QString temp1 = QString::number((int)ch1, 16);
				QString temp2 = QString::number((int)ch2, 16);
				QString temp3 = QString::number((int)ch3, 16);
				QString temp4 = QString::number((int)ch4, 16);
				if (temp1.size() == 1) temp1 = "0" + temp1;
				if (temp1 == "60") temp1 = "00";
				if (temp2.size() == 1) temp2 = "0" + temp2;
				if (temp2 == "60") temp2 = "00";
				if (temp3.size() == 1) temp3 = "0" + temp3;
				if (temp3 == "60") temp3 = "00";
				if (temp4.size() == 1) temp4 = "0" + temp4;
				if (temp4 == "60") temp4 = "00";
				out = temp4 + temp3 + temp2 + temp1;
			}
			else return false;
			bin_table->item((addr + i * 4) / 4, (addr + i * 4) % 4 + 4)->setText(out.mid(0, 2));
			bin_table->item((addr + i * 4) / 4, (addr + i * 4) % 4 + 3)->setText(out.mid(2, 2));
			bin_table->item((addr + i * 4) / 4, (addr + i * 4) % 4 + 2)->setText(out.mid(4, 2));
			bin_table->item((addr + i * 4) / 4, (addr + i * 4) % 4 + 1)->setText(out.mid(6, 2));
		}
	}
	return true;
}

bool Segment::_code_to_binary(QTableWidget *bin_table, QString &code, const QVector<Label> &label_list, int addr)
{
	int i;
	// 创建Instruction对象
	Instruction ins = Instruction(code, addr);
	// 转换为二进制码存入字符串
	QString res = ins.to_bin(label_list);
	if (res == "")
		return false;
	// 将得到的字符串写入二进制码表
	bin_table->item(addr / 4, addr % 4 + 1)->setText(res.mid(0, 2));
	bin_table->item(addr / 4, addr % 4 + 2)->setText(res.mid(2, 2));
	bin_table->item(addr / 4, addr % 4 + 3)->setText(res.mid(4, 2));
	bin_table->item(addr / 4, addr % 4 + 4)->setText(res.mid(6, 2));
	return true;
}