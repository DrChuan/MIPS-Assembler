//========================================================================================================================
// asm_text_highlight.cpp
//------------------------------------------------------------------------------------------------------------------------
// 此文件包含Highlighter类的方法定义
//========================================================================================================================

#include "asm_text_highlight.h"

Highlighter::Highlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
	HighlightingRule rule;
	// 一般文字：黑色
	normalFormat.setForeground(Qt::black);
	normalFormat.setFontFamily("Monaco");
	rule.pattern = QRegularExpression("[^\n]*");
	rule.format = normalFormat;
	highlightingRules.append(rule);
	// 数字：暗洋红色
	numberFormat.setForeground(Qt::darkMagenta);
	numberFormat.setFontFamily("Monaco");
	rule.pattern = QRegularExpression("\\b[0-9]+(\\s|;|,|$|\\()");
	rule.format = numberFormat;
	highlightingRules.append(rule);
	rule.pattern = QRegularExpression("\\b0x[0-9a-fA-F]+(\\s|;|,|$|\\()");
	highlightingRules.append(rule);
	// 指令：红色
	instructionFormat.setForeground(Qt::blue);
	instructionFormat.setFontFamily("Monaco");
	QStringList instructionPatterns;
	for (int i = 0; i < INSTRUCTION_NUM; i++)
		instructionPatterns << "\\b" + OPRTR_LC[i] + "\\b" << "\\b" + OPRTR_UC[i] + "\\b";
	rule.format = instructionFormat;
	foreach(const QString &pattern, instructionPatterns) {
		rule.pattern = QRegularExpression(pattern);
		highlightingRules.append(rule);
	}
	// 寄存器：淡蓝色
	registerFormat.setForeground(Qt::darkCyan);
	registerFormat.setFontFamily("Monaco");
	QStringList registerPatterns; 
	registerPatterns << "(^|\\s|,|\\()R[0-9]{1,2}(\\s|;|,|$|\\))";
	for (int i = 0; i < 32; i++)
		registerPatterns << "(^|\\s|,|\\()\\" + REGS[i] + "(\\s|;|,|$|\\))";
	rule.format = registerFormat;
	foreach(const QString &pattern, registerPatterns) {
		rule.pattern = QRegularExpression(pattern);
		highlightingRules.append(rule);
	}
	rule.pattern = QRegularExpression("[,;()]");
	rule.format = normalFormat;
	highlightingRules.append(rule);
	// 伪指令：暗黄色
	pesudoinsFormat.setForeground(Qt::darkYellow);
	pesudoinsFormat.setFontFamily("Monaco");
	QStringList pesudoinsPatterns;
	pesudoinsPatterns << "\\bdb\\b" << "\\bDB\\b" << "\\bdw\\b" << "\\bDW\\b" << "\\bdd\\b" << "\\bDD\\b"
					  << "\\bBaseAddre\\b" << "\\bDataAddre\\b" << "\\bEQU\\b" << "\\bequ\\b"
					  << "\\bresb\\b" << "\\bRESB\\b" << "\\bresw\\b" << "\\bRESW\\b" << "\\bresd\\b" << "\\bRESD\\b";
	rule.format = pesudoinsFormat;
	foreach(const QString &pattern, pesudoinsPatterns) {
		rule.pattern = QRegularExpression(pattern);
		highlightingRules.append(rule);
	}
	// 字符串：暗红色
	stringFormat.setForeground(Qt::darkRed);
	stringFormat.setFontFamily("Monaco");
	rule.pattern = QRegularExpression("\'.*\'");
	rule.format = stringFormat;
	highlightingRules.append(rule);
	// 注释：暗绿色
	CommentFormat.setForeground(Qt::darkGreen);
	CommentFormat.setFontFamily("Monaco");
	rule.pattern = QRegularExpression("//[^\n]*");
	rule.format = CommentFormat;
	highlightingRules.append(rule);
	rule.pattern = QRegularExpression("#[^\n]*");
	highlightingRules.append(rule);
}

void Highlighter::highlightBlock(const QString &text)
{
	foreach(const HighlightingRule &rule, highlightingRules) {
		QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
		while (matchIterator.hasNext()) {
			QRegularExpressionMatch match = matchIterator.next();
			setFormat(match.capturedStart(), match.capturedLength(), rule.format);
		}
	}
}