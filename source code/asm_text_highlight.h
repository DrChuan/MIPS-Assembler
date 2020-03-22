//========================================================================================================================
// asm_text_highlight.h
//------------------------------------------------------------------------------------------------------------------------
// ���ļ�����Highlighter��Ķ��塣����̳���Qt���õ�QSyntaxHighlighter�࣬���ڻ����д����﷨����
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
	void highlightBlock(const QString &text) override; // ��дQSyntaxHighlighter�麯��

private:
	// �﷨���򣬰���������ʽģʽ����ƥ�����ʽ  
	struct HighlightingRule  
	{
		QRegularExpression pattern;
		QTextCharFormat    format;
	};
	// �﷨�����б����Զ�������������
	QVector<HighlightingRule> highlightingRules;
	// ��ͬ�﷨���Ͷ�Ӧ�������ʽ
	QTextCharFormat normalFormat;      // һ������
	QTextCharFormat numberFormat;	   // ����
	QTextCharFormat instructionFormat; // ָ��
	QTextCharFormat pesudoinsFormat;   // αָ��
	QTextCharFormat registerFormat;    // �Ĵ���
	QTextCharFormat CommentFormat;     // ע��
	QTextCharFormat stringFormat;      // �ַ���
};
