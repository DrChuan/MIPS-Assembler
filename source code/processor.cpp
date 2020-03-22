//========================================================================================================================
// processor.cpp
//------------------------------------------------------------------------------------------------------------------------
// ���ļ�����Processor��Ĺ��С�˽�з�������
//========================================================================================================================

#include "processor.h"

Processor::Processor(QTableWidget *_bin_table, QTextEdit *_asm_text, int _num_instructions) :
	bin_table(_bin_table), asm_text(_asm_text), num_instructions(_num_instructions){}

int Processor::assemble()
{
	// ��ȡ��������е�����
	QString asm_code = asm_text->toPlainText();
	// ��������е�ע��
	_remove_comment(asm_code);
	// Ԥ��ȡ��ǩ���ֶ�
	QVector<Segment> segments;
	QVector<Label> label_list = _read_label(asm_code, segments);
	// ����֮���Ƿ���ڸ��ǣ����У��򷵻�0���������ִ��
	int n = segments.size();
	for (int i = 0; i < n - 1; i++)
		if (segments[i].get_length() + segments[i].get_init_addr() > segments[i + 1].get_init_addr())
			return 0;
	// ����ܳ�����û�г������ޣ����У��򷵻�0���������ִ��
	if (segments[n - 1].get_length() + segments[n - 1].get_init_addr() > MAX_INSTRUCTIONS)
		return 0;
	// ��ÿ���ν��л��
	int count = 0;
	for (int i = 0; i < n; i++)
	{
		int success = segments[i].to_binary(bin_table, label_list);
		// ���ɹ���ͳ������ɻ�������
		if (success == -1)
			count += segments[i].get_length();
		// ��ʧ�ܣ����س��������
		else
			return count + success + i;
	}
	return -1;
}


int Processor::dissemble()
{
	int code, t;
	QString asm_instr;
	// ��ȡ��ǰʱ��
	QDateTime dt = QDateTime::currentDateTime();
	// �ڷ��������ͷ�����ʱ����Ϣ
	QString result = "#===========================================================================================\
                    \n# Dissembled at " + dt.toString("yyyy-MM-dd hh:mm:ss") +
		           "\n#===========================================================================================\n\n";
	// ����ָ����
	for (int i = 0; i < num_instructions; i++)
	{
		code = 0;
		// �ӱ��ж�ȡһ��word���ӵ͵�ַ��ʼ��ȡ���������λ�����ƣ�����
		for (int j = 1; j <= 4; j++)
		{
			t = bin_table->item(i, j)->text().toInt((bool*)nullptr, 16);
			code <<= 8;
			code |= t;
		}
		// �ɶ�ȡ����word����ָ�����
		Instruction ins = Instruction(code);
		// ���������Ϊ�գ��������ʧ�ܣ����س��������
		if ((asm_instr = ins.to_asm()) == "")
			return i + 1;
		// �������ɹ�������д�����ַ���
		result = result + asm_instr + ";\n";
	}
	// ������ַ���д�����
	asm_text->setText(result);
	return -1;
}



void Processor::_remove_comment(QString &asm_code)
{
	asm_code = asm_code.remove(QRegularExpression("//[^\n]*"));
	asm_code = asm_code.remove(QRegularExpression("#[^\n]*"));
}


QVector<Label> Processor::_read_label(QString &asm_code, QVector<Segment> &segments)
{
	QVector<Label> label_list;
	// ��Դ����ֶ�
	_set_segment(segments, asm_code);
	// ����ÿ����
	for (int i = 0; i < segments.size(); i++)
	{
		// �������е�ÿһ�����
		for (int j = 0; j < segments[i].get_sentences().size(); j++)
		{
			Sentence &s = segments[i].get_sentences()[j];
			// �����Ƿ��б�ǩ
			int findlabel = s.get_code().indexOf(':', 0);
			if (findlabel != -1)
			{
				QString labelname = s.get_code().left(findlabel);
				labelname.remove(' ');
				label_list.push_back(Label(labelname, segments[i].get_init_addr() + s.get_addr()));
				s.remove_label();
			}
		}
	}
	return label_list;
}

void Processor::_set_segment(QVector<Segment> &segments, QString &asm_code)
{
	asm_code = "BaseAddre:00000000;\n" + asm_code;
	// �ڶα�Ǵ���������/!/!
	asm_code.replace("DataAddre:", "/!/!DataAddre:");
	asm_code.replace("BaseAddre:", "/!/!BaseAddre:");
	// ���������ǲ��ֶ�
	QStringList seg_list = asm_code.split("/!/!");
	for (int i = 1; i < seg_list.size(); i++)
	{
		Segment *t = new Segment(seg_list[i]);
		segments.push_back(*t);
	}
}