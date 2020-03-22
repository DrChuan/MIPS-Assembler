//========================================================================================================================
// processor.cpp
//------------------------------------------------------------------------------------------------------------------------
// 此文件包含Processor类的公有、私有方法定义
//========================================================================================================================

#include "processor.h"

Processor::Processor(QTableWidget *_bin_table, QTextEdit *_asm_text, int _num_instructions) :
	bin_table(_bin_table), asm_text(_asm_text), num_instructions(_num_instructions){}

int Processor::assemble()
{
	// 获取汇编代码框中的内容
	QString asm_code = asm_text->toPlainText();
	// 清除代码中的注释
	_remove_comment(asm_code);
	// 预读取标签及分段
	QVector<Segment> segments;
	QVector<Label> label_list = _read_label(asm_code, segments);
	// 检查段之间是否存在覆盖，若有，则返回0，否则继续执行
	int n = segments.size();
	for (int i = 0; i < n - 1; i++)
		if (segments[i].get_length() + segments[i].get_init_addr() > segments[i + 1].get_init_addr())
			return 0;
	// 检查总长度有没有超过上限，若有，则返回0，否则继续执行
	if (segments[n - 1].get_length() + segments[n - 1].get_init_addr() > MAX_INSTRUCTIONS)
		return 0;
	// 对每个段进行汇编
	int count = 0;
	for (int i = 0; i < n; i++)
	{
		int success = segments[i].to_binary(bin_table, label_list);
		// 若成功，统计已完成汇编的行数
		if (success == -1)
			count += segments[i].get_length();
		// 若失败，返回出错的行数
		else
			return count + success + i;
	}
	return -1;
}


int Processor::dissemble()
{
	int code, t;
	QString asm_instr;
	// 获取当前时间
	QDateTime dt = QDateTime::currentDateTime();
	// 在反汇编结果的头部添加时间信息
	QString result = "#===========================================================================================\
                    \n# Dissembled at " + dt.toString("yyyy-MM-dd hh:mm:ss") +
		           "\n#===========================================================================================\n\n";
	// 逐条指令反汇编
	for (int i = 0; i < num_instructions; i++)
	{
		code = 0;
		// 从表中读取一个word。从低地址开始读取，置入最低位，左移，往复
		for (int j = 1; j <= 4; j++)
		{
			t = bin_table->item(i, j)->text().toInt((bool*)nullptr, 16);
			code <<= 8;
			code |= t;
		}
		// 由读取到的word创建指令对象
		Instruction ins = Instruction(code);
		// 若反汇编结果为空，即反汇编失败，返回出错的行数
		if ((asm_instr = ins.to_asm()) == "")
			return i + 1;
		// 若反汇编成功，将其写入结果字符串
		result = result + asm_instr + ";\n";
	}
	// 将结果字符串写入汇编框
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
	// 将源代码分段
	_set_segment(segments, asm_code);
	// 遍历每个段
	for (int i = 0; i < segments.size(); i++)
	{
		// 遍历段中的每一条语句
		for (int j = 0; j < segments[i].get_sentences().size(); j++)
		{
			Sentence &s = segments[i].get_sentences()[j];
			// 查找是否有标签
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
	// 在段标记处做特殊标记/!/!
	asm_code.replace("DataAddre:", "/!/!DataAddre:");
	asm_code.replace("BaseAddre:", "/!/!BaseAddre:");
	// 查找特殊标记并分段
	QStringList seg_list = asm_code.split("/!/!");
	for (int i = 1; i < seg_list.size(); i++)
	{
		Segment *t = new Segment(seg_list[i]);
		segments.push_back(*t);
	}
}