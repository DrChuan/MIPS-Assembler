//========================================================================================================================
// asm_text_highlight.h
//------------------------------------------------------------------------------------------------------------------------
// 此文件包括Highlighter类的定义。该类继承自Qt内置的QSyntaxHighlighter类，用于汇编框中代码语法高亮
//========================================================================================================================

#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <qcoreapplication.h>
#include "regs_and_oprtrs.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class Highlighter : public QSyntaxHighlighter
{
	Q_OBJECT

public:
	Highlighter(QTextDocument *parent = 0);

protected:
	void highlightBlock(const QString &text) override; // 重写QSyntaxHighlighter虚函数

private:
	// 语法规则，包含正则表达式模式串和匹配的样式  
	struct HighlightingRule  
	{
		QRegularExpression pattern;
		QTextCharFormat    format;
	};
	// 语法规则列表，可以定义多个高亮规则
	QVector<HighlightingRule> highlightingRules;
	// 不同语法类型对应的字体格式
	QTextCharFormat normalFormat;      // 一般文字
	QTextCharFormat numberFormat;	   // 数字
	QTextCharFormat instructionFormat; // 指令
	QTextCharFormat pesudoinsFormat;   // 伪指令
	QTextCharFormat registerFormat;    // 寄存器
	QTextCharFormat CommentFormat;     // 注释
	QTextCharFormat stringFormat;      // 字符串
};
