//========================================================================================================================
// instruction.cpp
//------------------------------------------------------------------------------------------------------------------------
// ���ļ�����Instruction��Ĺ��С�˽�з�������
//========================================================================================================================

#include "processor.h"

// ����num[begin:end]
int get_bit(unsigned int num, int begin, int end)
{
	num >>= end;
	num <<= end + 31 - begin;
	num >>= end + 31 - begin;
	return num;
}

// ��dst��[begin:end]��Ϊsrc
void set_bit(unsigned int &dst, int src, int begin, int end)
{
	int t = get_bit(src, begin - end, 0);
	t <<= end;
	dst |= t;
}

Instruction::Instruction(int code)
{
	intcode = code;
	// ��ȡ������
	op = get_bit(code, 31, 26);
	// ����ȷ��ָ������к�
	for (idx = 0; idx < INSTRUCTION_NUM; idx++)
		if (get_bit(OP_OR_FUNC[idx], 7, 0) == op && (op != 0 || get_bit(code, 5, 0) == get_bit(OP_OR_FUNC[idx], 15, 8)))
			break;
	if (idx == INSTRUCTION_NUM) op = -1;
	// ����ָ������
	_set_type();
	// R��ָ���ת��
	if (type == R)
	{
		rs = get_bit(code, 25, 21);
		rt = get_bit(code, 20, 16);
		rd = get_bit(code, 15, 11);
		shamt = get_bit(code, 10, 6);
		func = get_bit(code, 5, 0);
	}
	// I��ָ���ת��
	else if (type == I)
	{
		rs = get_bit(code, 25, 21);
		rt = get_bit(code, 20, 16);
		imm = get_bit(code, 15, 0);
	}
	// J��ָ���ת��
	else if (type == J)
	{
		addr = get_bit(code, 25, 0);
	}
}

Instruction::Instruction(QString code, int _addr)
{
	ins_addr = _addr;
	this->code = code;
	idx = 0;
	// ��ָ������������ζԱȣ����Ҳ����򷵻ش���
	while (idx < INSTRUCTION_NUM && !code.startsWith(OPRTR_LC[idx] + " ", Qt::CaseInsensitive)) idx++;
	if (idx == INSTRUCTION_NUM) return;
	// ����idx����ָ�������
	_set_type();
	// ��ΪR�ͣ�����op��func��ֵ
	if (type == R)
	{
		op = get_bit(OP_OR_FUNC[idx], 7, 0);
		func = get_bit(OP_OR_FUNC[idx], 15, 8);
	}
	// ��ΪI�ͻ�J�ͣ�����op��ֵ
	else if (type == I || type == J)
		op = OP_OR_FUNC[idx];
}

// ����idxͨ�����ұ�����ָ�������
void Instruction::_set_type()
{
	if (IS_RTYPE[idx])
		type = R;
	else if (IS_ITYPE[idx])
		type = I;
	else if (IS_JTYPE[idx])
		type = J;
	else
		type = Pesudo;
}

// ���ݼĴ���������ͨ�����ұ��������
int Instruction::_reg_num(const QString &reg_name)
{
	for (int i = 0; i < 32; i++)
		if (REGS[i] == reg_name || "R" + QString::number(i) == reg_name)
			return i;
	return -1;
}

// ******������ָ��ת��Ϊ������ĺ���*******
QString Instruction::to_asm()
{
	QString result;
	// ��ָ����д�����ַ���
	result = OPRTR_LC[idx] + "   ";
	// ��ָ��Ϸ����򷵻�
	if (op == -1)
		return "";
	else if (op == -2)
	{
		// ռ��
	}
	// �����ȡָ��
	else if (idx >= 0 && idx <= 7)
		result += (REGS[rt] + ", 0x" + QString::number(imm, 16) + "(" + REGS[rs] + ")");
	// �����ָ֧��
	else if (idx == 33 || idx == 34)
		result += (REGS[rs] + ", " + REGS[rt] + ", 0x" + QString::number(imm << 2, 16));
	else if (idx == 35 || idx == 36)
		result += (REGS[rs] + ", 0x" + QString::number(imm << 2, 16));
	// ������λָ��
	else if (idx == 18 || idx == 19 || idx == 20)
		result += (REGS[rd] + ", " + REGS[rt] + ", " + QString::number(shamt, 10));
	// ����Ĵ�����תָ��
	else if (idx == 41 || idx == 42)
		result += REGS[rs];
	// ����һ��R��ָ��
	else if (type == R)
		result += (REGS[rd] + ", " + REGS[rs] + ", " + REGS[rt]);
	// ����һ��I��ָ��
	else if (type == I)
		result += (REGS[rt] + ", " + REGS[rs] + ", 0x" + QString::number(imm, 16));
	// ����J��ָ��
	else if (type == J)
		result += QString::number(addr, 16);
	return result;
}

// ***************��������ת���ɻ���ָ��ĺ���****************
QString Instruction::to_bin(const QVector<Label> &label_list)
{
	if (idx == INSTRUCTION_NUM) return "";
	if (op == -2) // ��ʱռ�ӡ���ʾ�Ǳ�׼��ָ���״��
	{

	}
	// �����һ���R��ָ��
	else if (type == R)
	{
		// ���������Ƴ�
		code.remove(OPRTR_LC[idx], Qt::CaseInsensitive);
		// �Ƴ��ո�
		code.remove(' ');
		// ���������
		QStringList oprands = code.split(',');
		// ����jr��jalrָ��
		if (idx == 41 || idx == 42)
		{
			oprands.push_front("R0");
			oprands.push_back("R0");
		}
		// �����������Ϊ3�������󷵻�
		if (oprands.size() != 3)
			return "";
		// ��ȡ�����������Ĵ����ı�ţ����������������󷵻�
		// ���⴦����λָ��
		if (idx == 18 || idx == 19 || idx == 20)
		{
			if ((rd = _reg_num(oprands[0])) == -1) return "";
			if ((rt = _reg_num(oprands[1])) == -1) return "";
			// ת��������������������󷵻أ�����ת��Ϊint
			bool ok;
			shamt = oprands[2].toInt(&ok, 10);
			if (!ok && oprands[2].startsWith("0x"))
				shamt = oprands[2].remove("0x").toInt(&ok, 16);
			if (!ok) return "";
			if (shamt > 31 || shamt < 0) return "";
			rs = 0;
		}
		else
		{
			if ((rd = _reg_num(oprands[0])) == -1) return "";
			if ((rs = _reg_num(oprands[1])) == -1) return "";
			if ((rt = _reg_num(oprands[2])) == -1) return "";
			shamt = 0;
		}
		// ���������ְ�λ������
		unsigned int t = 0;
		set_bit(t, func, 5, 0);
		set_bit(t, rs, 25, 21);
		set_bit(t, rt, 20, 16);
		set_bit(t, rd, 15, 11);
		set_bit(t, shamt, 10, 6);
		// �����ת��Ϊ�ַ���������
		QString result = QString::number(t, 16);
		result = QString(8 - result.size(), '0') + result;
		return result;
	}
	else if (type == I)
	{
		// ���������Ƴ�
		code.remove(OPRTR_LC[idx], Qt::CaseInsensitive);
		// �Ƴ��ո�
		code.remove(' ');
		// ���������
		code.replace('(', ',');
		code.remove(')');
		// ���⴦��blez��bgtzָ��
		if (idx == 35 || idx == 36)
			code = "R0," + code;
		QStringList oprands = code.split(',');
		// ���⴦���дָ��
		if (idx >= 0 && idx <= 7)
			oprands.swap(1, 2);
		// ���⴦���ָ֧���rs��rt��
		if (idx == 33 || idx == 34)
			oprands.swap(0, 1);
		// �����������Ϊ3�������󷵻�
		if (oprands.size() != 3)
			return "";
		// ���⴦������������ΪLabel��ָ���дָ��ͷ�ָ֧�
		if ((idx >= 0 && idx <= 7) || (idx >= 33 && idx <= 36))
		{
			int n = label_list.size();
			for (int i = 0; i < n; i++)
				if (oprands[2] == label_list[i].get_name())
				{
					oprands[2] = QString::number((short int)(label_list[i].get_addr() - ins_addr));
					break;
				}
		}
		// ��ȡ�����Ĵ����ı�ţ����������������󷵻�
		if ((rt = _reg_num(oprands[0])) == -1) return "";
		if ((rs = _reg_num(oprands[1])) == -1) return "";
		// ת��������������������󷵻أ�����ת��Ϊint
		bool ok;
		imm = oprands[2].toInt(&ok, 10);
		if (!ok && oprands[2].startsWith("0x"))
			imm = oprands[2].remove("0x").toInt(&ok, 16);
		if (!ok) return "";
		if (imm > 32767 || imm < -32768) return "";
		if (idx >= 33 && idx <= 36)
			imm >>= 2;
		// ���������ְ�λ������
		unsigned int t = 0;
		set_bit(t, op, 31, 26);
		set_bit(t, rs, 25, 21);
		set_bit(t, rt, 20, 16);
		set_bit(t, (short int)imm, 15, 0);
		// �����ת��Ϊ�ַ���������
		QString result = QString::number(t, 16);
		result = QString(8 - result.size(), '0') + result;
		return result;
	}
	else if (type == J)
	{
		// ���������Ƴ�
		code.remove(OPRTR_LC[idx], Qt::CaseInsensitive);
		// �Ƴ��ո�
		code.remove(' ');
		// ���������
		QStringList oprands = code.split(',');
		// �����������Ϊ1�������󷵻�
		if (oprands.size() != 1)
			return "";
		QString opr = oprands[0];
		// ������ǩ�����������ƥ��
		int n = label_list.size();
		addr = -1;
		for (int i = 0; i < n; i++)
			if (opr == label_list[i].get_name())
				addr = label_list[i].get_addr();
		// ��û�ҵ������ж����Ƿ�Ϊһ������
		if (addr == -1)
		{
			bool ok;
			addr = opr.toInt(&ok, 10);
			if (!ok && opr.startsWith("0x"))
				addr = opr.remove("0x").toInt(&ok, 16);
			if (!ok) return "";
			if (addr > 0xfffffff || addr < 0) return "";
		}
		// �����Ǳ�ǩҲ�������֣����󷵻�
		if (addr == -1) return "";
		// ��λд����
		unsigned int t = 0;
		set_bit(t, op, 31, 26);
		set_bit(t, addr >> 2, 25, 0);
		// �����ת��Ϊ�ַ��������
		QString result = QString::number(t, 16);
		result = QString(8 - result.size(), '0') + result;
		return result;
	}
	return "";
}