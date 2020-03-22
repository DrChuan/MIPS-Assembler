//========================================================================================================================
// asm_text_highlight.cpp
//------------------------------------------------------------------------------------------------------------------------
// ���ļ�����Highlighter��ķ�������
//========================================================================================================================

#include "asm_text_highlight.h"

Highlighter::Highlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
	HighlightingRule rule;
	// һ�����֣���ɫ
	normalFormat.setForeground(Qt::black);
	normalFormat.setFontFamily("Monaco");
	rule.pattern = QRegularExpression("[^\n]*");
	rule.format = normalFormat;
	highlightingRules.append(rule);
	// ���֣������ɫ
	numberFormat.setForeground(Qt::darkMagenta);
	numberFormat.setFontFamily("Monaco");
	rule.pattern = QRegularExpression("\\b[0-9]+(\\s|;|,|$|\\()");
	rule.format = numberFormat;
	highlightingRules.append(rule);
	rule.pattern = QRegularExpression("\\b0x[0-9a-fA-F]+(\\s|;|,|$|\\()");
	highlightingRules.append(rule);
	// ָ���ɫ
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
	// �Ĵ���������ɫ
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
	// αָ�����ɫ
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
	// �ַ���������ɫ
	stringFormat.setForeground(Qt::darkRed);
	stringFormat.setFontFamily("Monaco");
	rule.pattern = QRegularExpression("\'.*\'");
	rule.format = stringFormat;
	highlightingRules.append(rule);
	// ע�ͣ�����ɫ
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