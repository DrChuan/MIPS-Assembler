//========================================================================================================================
// segment.cpp
//------------------------------------------------------------------------------------------------------------------------
// ���ļ�����Segment��Ĺ��С�˽�з�������
//========================================================================================================================

#include "processor.h"

Segment::Segment(const QString &source)
{
	QStringList code;
	// �ж�Ϊ����λ������ݶ�
	is_code = (source.left(4) == "Base");
	// �����δ��밴�ֺŷָ������
	code = source.split(";");
	QString head = code[0];
	// ��ȡ�λ�ַ
	head.remove("BaseAddre:");
	head.remove("DataAddre:");
	head.remove(' ');
	bool ok;
	int init = head.toInt(&ok, 16);
	// ���λ�ַ���ǺϷ���ʮ��������������Ϊ0x00000000����
	if (!ok)
		init_addr = 0;
	else
		init_addr = init;
	// �������öε�αָ���Ƴ�
	code.removeFirst();
	code.removeLast();
	// �������ַ���ת���������󣬲�ʵʱ���¶γ���
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
	// ��ȡ�öε������
	int n = sentences.size();
	// ������䣬ִ�л��
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
	// ��ʱ�ַ���
	QString t = _code;
	// �Ƴ���ǩ����
	t.remove(QRegularExpression("[^\n]*:"));
	// �Ƴ��ո�
	t.remove(' ');
	// �ж��Ƿ�Ϊ���ݶ��壬���ǣ�������Ӧ�ĳ��ȣ������ǣ�������Ϊ4
	if (t.startsWith("DB") || t.startsWith("db"))
	{
		data_type = 1;
		// �����ַ���
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
	length = (((t.count(',') + 1) * data_type - 1) / 4 + 1) * 4;  // �ֽڶ��룬ÿ������������һ��word�е�ʣ���ֽ�
}

void Sentence::remove_label()
{
	code.remove(QRegularExpression("[^\n]*:"));
}