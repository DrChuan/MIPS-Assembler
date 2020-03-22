//========================================================================================================================
// processor.h
//------------------------------------------------------------------------------------------------------------------------
// 此文件包括五个类的声明：
// - Instruction类：表示一条机器指令
// - Label类：表示汇编代码中的一个标签
// - Sentence类：表示一条汇编语句，包括内容、偏移地址和长度
// - Segment类：表示汇编代码中的一段
// - Processor类：执行汇编和反汇编的处理器类，暴露给主程序调用
//========================================================================================================================

#pragma once
#include <qtablewidget.h>
#include <qtextedit.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qvector.h>
#include "myAssembler.h"
#include "regs_and_oprtrs.h"

// ----------------------------------------------------------------------------------------------------------
// Label类
// ----------------------------------------------------------------------------------------------------------
class Label
{
public:
	Label(QString _name, int _addr) : name(_name), address(_addr) {}
	QString get_name() const { return name; }
	int     get_addr() const { return address; }
private:
	QString name;
	int     address;
};

// ----------------------------------------------------------------------------------------------------------
// Instruction类
// ----------------------------------------------------------------------------------------------------------
class Instruction
{
public:
	        Instruction(int code);
			Instruction(QString code, int _addr);
	QString to_asm();
	QString to_bin(const QVector<Label> &label_list);
private:
	enum    ins_type {R, I, J, Pesudo} type;
	int     op;
	int     rs, rt, rd;
	int     shamt;
	int     func;
	int     imm;
	int     addr;
	QString code;
	int     intcode;
	int     idx;
	int     ins_addr;
	void    _set_type();
	int     _reg_num(const QString &reg_name);
};

// ----------------------------------------------------------------------------------------------------------
// Sentence类
// ----------------------------------------------------------------------------------------------------------
class Sentence
{
public:
	        Sentence(QString &_code, int _addr);
	QString get_code() const { return code; };
	int     get_addr() const { return addr; };
	int     get_len() const { return length; };
	int     get_datatype() const { return data_type; };
	void    remove_label();
private:
	QString code;
	int     addr;
	int     length;
	int     data_type;
};

// ----------------------------------------------------------------------------------------------------------
// Segment类
// ----------------------------------------------------------------------------------------------------------
class Segment
{
public:
	                  Segment(const QString &source);
	int               get_length() const;
	int               get_init_addr() const;
	bool              is_code_seg() const;
	QVector<Sentence>&get_sentences();
	int               to_binary(QTableWidget *bin_table, const QVector<Label> &label_list);

private:
	QVector<Sentence> sentences;
	bool              is_code;
	int               init_addr;
	int               length;
	bool              _data_to_binary(QTableWidget *bin_table, QString &code, int addr, int data_type);
	bool              _code_to_binary(QTableWidget *bin_table, QString &code, const QVector<Label> &label_list, int addr);
};



// ----------------------------------------------------------------------------------------------------------
// Processor类
// ----------------------------------------------------------------------------------------------------------
class Processor
{
public:
	    Processor(QTableWidget *_bin_table, QTextEdit *_asm_text, int _num_instructions);
	int assemble();
	int dissemble();

private:
	QTableWidget *bin_table;
	QTextEdit    *asm_text;
	int           num_instructions;

	void           _remove_comment(QString &asm_code);
	QVector<Label> _read_label(QString &asm_code, QVector<Segment> &segments);
	void           _set_segment(QVector<Segment> &segments, QString &asm_code);
};

