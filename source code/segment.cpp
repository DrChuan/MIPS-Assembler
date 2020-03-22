//========================================================================================================================
// segment.cpp
//------------------------------------------------------------------------------------------------------------------------
// 此文件包含Segment类的公有、私有方法定义
//========================================================================================================================

#include "processor.h"

Segment::Segment(const QString &source)
{
	QStringList code;
	// 判断为代码段还是数据段
	is_code = (source.left(4) == "Base");
	// 将整段代码按分号分隔成语句
	code = source.split(";");
	QString head = code[0];
	// 读取段基址
	head.remove("BaseAddre:");
	head.remove("DataAddre:");
	head.remove(' ');
	bool ok;
	int init = head.toInt(&ok, 16);
	// 若段基址不是合法的十六进制数，则作为0x00000000处理
	if (!ok)
		init_addr = 0;
	else
		init_addr = init;
	// 将声明该段的伪指令移除
	code.removeFirst();
	code.removeLast();
	// 将代码字符串转化成语句对象，并实时更新段长度
	length = 0;
	for (int i = 0; i < code.size(); i++)
	{
		code[i].replace("equ", ":dd", Qt::CaseInsensitive);
		code[i].trimmed();
		code[i].remove('\n');
		sentences.push_back(Sentence(code[i], length));
		length += sentences[i].get_len();
	}
}

int Segment::get_length() const
{
	return length;
}

int Segment::get_init_addr() const
{
	return init_addr;
}

bool Segment::is_code_seg() const
{
	return is_code;
}

QVector<Sentence> &Segment::get_sentences()
{
	return sentences;
}

int Segment::to_binary(QTableWidget *bin_table, const QVector<Label> &label_list)
{
	// 获取该段的语句数
	int n = sentences.size();
	// 遍历语句，执行汇编
	if (is_code)
		for (int i = 0; i < n; i++)
		{
			QString code = sentences[i].get_code();
			code = code.trimmed();
			if (!_code_to_binary(bin_table, code, label_list, init_addr + sentences[i].get_addr()))
				return i + 1;
		}
	else
		for (int i = 0; i < n; i++)
		{
			QString code = sentences[i].get_code();
			code.trimmed();
			if (!_data_to_binary(bin_table, code, init_addr + sentences[i].get_addr(), sentences[i].get_datatype()))
				return i + 1;
		}
	return -1;
}

Sentence::Sentence(QString &_code, int _addr)
{
	code = _code;
	addr = _addr;
	// 临时字符串
	QString t = _code;
	// 移除标签部分
	t.remove(QRegularExpression("[^\n]*:"));
	// 移除空格
	t.remove(' ');
	// 判断是否为数据定义，若是，设置相应的长度，若不是，长度置为4
	if (t.startsWith("DB") || t.startsWith("db"))
	{
		data_type = 1;
		// 处理字符串
		while (code.contains('\'') == true)
		{
			code[code.indexOf('\'')] = '|';
			code[code.indexOf('\'')] = '|';
			QStringList tmp = code.split('|');
			QString t = tmp[0];
			int n = tmp[1].size();
			for (int i = 0; i < n - 1; i++)
				t += ("/" + tmp[1][i] + "/, ");
			t += ("/" + tmp[1][n - 1] + "/");
			t += tmp[2];
			code = t;
		}
		code.replace('/', '\'');
	}
	else if (t.startsWith("DW") || t.startsWith("dw"))
		data_type = 2;
	else if (t.startsWith("DD") || t.startsWith("dd"))
		data_type = 4;
	else
	{
		length = 4;
		return;
	}
	length = (((t.count(',') + 1) * data_type - 1) / 4 + 1) * 4;  // 字节对齐，每条语句舍弃最后一个word中的剩余字节
}

void Sentence::remove_label()
{
	code.remove(QRegularExpression("[^\n]*:"));
}