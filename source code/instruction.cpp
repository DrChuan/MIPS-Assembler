//========================================================================================================================
// instruction.cpp
//------------------------------------------------------------------------------------------------------------------------
// 此文件包含Instruction类的公有、私有方法定义
//========================================================================================================================

#include "processor.h"

// 返回num[begin:end]
int get_bit(unsigned int num, int begin, int end)
{
	num >>= end;
	num <<= end + 31 - begin;
	num >>= end + 31 - begin;
	return num;
}

// 将dst的[begin:end]置为src
void set_bit(unsigned int &dst, int src, int begin, int end)
{
	int t = get_bit(src, begin - end, 0);
	t <<= end;
	dst |= t;
}

Instruction::Instruction(int code)
{
	intcode = code;
	// 获取操作码
	op = get_bit(code, 31, 26);
	// 遍历确定指令的序列号
	for (idx = 0; idx < INSTRUCTION_NUM; idx++)
		if (get_bit(OP_OR_FUNC[idx], 7, 0) == op && (op != 0 || get_bit(code, 5, 0) == get_bit(OP_OR_FUNC[idx], 15, 8)))
			break;
	if (idx == INSTRUCTION_NUM) op = -1;
	// 设置指令类型
	_set_type();
	// R型指令的转化
	if (type == R)
	{
		rs = get_bit(code, 25, 21);
		rt = get_bit(code, 20, 16);
		rd = get_bit(code, 15, 11);
		shamt = get_bit(code, 10, 6);
		func = get_bit(code, 5, 0);
	}
	// I型指令的转化
	else if (type == I)
	{
		rs = get_bit(code, 25, 21);
		rt = get_bit(code, 20, 16);
		imm = get_bit(code, 15, 0);
	}
	// J型指令的转化
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
	// 将指令与操作符依次对比，若找不到则返回错误
	while (idx < INSTRUCTION_NUM && !code.startsWith(OPRTR_LC[idx] + " ", Qt::CaseInsensitive)) idx++;
	if (idx == INSTRUCTION_NUM) return;
	// 根据idx设置指令的类型
	_set_type();
	// 若为R型，设置op和func的值
	if (type == R)
	{
		op = get_bit(OP_OR_FUNC[idx], 7, 0);
		func = get_bit(OP_OR_FUNC[idx], 15, 8);
	}
	// 若为I型或J型，设置op的值
	else if (type == I || type == J)
		op = OP_OR_FUNC[idx];
}

// 根据idx通过查找表设置指令的类型
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

// 根据寄存器的名字通过查找表搜索编号
int Instruction::_reg_num(const QString &reg_name)
{
	for (int i = 0; i < 32; i++)
		if (REGS[i] == reg_name || "R" + QString::number(i) == reg_name)
			return i;
	return -1;
}

// ******将机器指令转化为汇编语句的函数*******
QString Instruction::to_asm()
{
	QString result;
	// 将指令名写入结果字符串
	result = OPRTR_LC[idx] + "   ";
	// 若指令不合法，则返回
	if (op == -1)
		return "";
	else if (op == -2)
	{
		// 占坑
	}
	// 处理存取指令
	else if (idx >= 0 && idx <= 7)
		result += (REGS[rt] + ", 0x" + QString::number(imm, 16) + "(" + REGS[rs] + ")");
	// 处理分支指令
	else if (idx == 33 || idx == 34)
		result += (REGS[rs] + ", " + REGS[rt] + ", 0x" + QString::number(imm << 2, 16));
	else if (idx == 35 || idx == 36)
		result += (REGS[rs] + ", 0x" + QString::number(imm << 2, 16));
	// 处理移位指令
	else if (idx == 18 || idx == 19 || idx == 20)
		result += (REGS[rd] + ", " + REGS[rt] + ", " + QString::number(shamt, 10));
	// 处理寄存器跳转指令
	else if (idx == 41 || idx == 42)
		result += REGS[rs];
	// 处理一般R型指令
	else if (type == R)
		result += (REGS[rd] + ", " + REGS[rs] + ", " + REGS[rt]);
	// 处理一般I型指令
	else if (type == I)
		result += (REGS[rt] + ", " + REGS[rs] + ", 0x" + QString::number(imm, 16));
	// 处理J型指令
	else if (type == J)
		result += QString::number(addr, 16);
	return result;
}

// ***************将汇编语句转化成机器指令的函数****************
QString Instruction::to_bin(const QVector<Label> &label_list)
{
	if (idx == INSTRUCTION_NUM) return "";
	if (op == -2) // 临时占坑。表示非标准的指令的状况
	{

	}
	// 如果是一般的R型指令
	else if (type == R)
	{
		// 将操作符移除
		code.remove(OPRTR_LC[idx], Qt::CaseInsensitive);
		// 移除空格
		code.remove(' ');
		// 分离操作数
		QStringList oprands = code.split(',');
		// 处理jr和jalr指令
		if (idx == 41 || idx == 42)
		{
			oprands.push_front("R0");
			oprands.push_back("R0");
		}
		// 如果操作数不为3个，错误返回
		if (oprands.size() != 3)
			return "";
		// 获取三个操作数寄存器的编号，如果有误，则给出错误返回
		// 特殊处理移位指令
		if (idx == 18 || idx == 19 || idx == 20)
		{
			if ((rd = _reg_num(oprands[0])) == -1) return "";
			if ((rt = _reg_num(oprands[1])) == -1) return "";
			// 转化立即数，若有误则错误返回，否则转化为int
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
		// 将各个部分按位置入结果
		unsigned int t = 0;
		set_bit(t, func, 5, 0);
		set_bit(t, rs, 25, 21);
		set_bit(t, rt, 20, 16);
		set_bit(t, rd, 15, 11);
		set_bit(t, shamt, 10, 6);
		// 将结果转化为字符串并返回
		QString result = QString::number(t, 16);
		result = QString(8 - result.size(), '0') + result;
		return result;
	}
	else if (type == I)
	{
		// 将操作符移除
		code.remove(OPRTR_LC[idx], Qt::CaseInsensitive);
		// 移除空格
		code.remove(' ');
		// 分离操作数
		code.replace('(', ',');
		code.remove(')');
		// 特殊处理blez和bgtz指令
		if (idx == 35 || idx == 36)
			code = "R0," + code;
		QStringList oprands = code.split(',');
		// 特殊处理读写指令
		if (idx >= 0 && idx <= 7)
			oprands.swap(1, 2);
		// 特殊处理分支指令（先rs后rt）
		if (idx == 33 || idx == 34)
			oprands.swap(0, 1);
		// 如果操作数不为3个，错误返回
		if (oprands.size() != 3)
			return "";
		// 特殊处理立即数可以为Label的指令（读写指令和分支指令）
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
		// 获取两个寄存器的编号，如果有误，则给出错误返回
		if ((rt = _reg_num(oprands[0])) == -1) return "";
		if ((rs = _reg_num(oprands[1])) == -1) return "";
		// 转化立即数，若有误则错误返回，否则转化为int
		bool ok;
		imm = oprands[2].toInt(&ok, 10);
		if (!ok && oprands[2].startsWith("0x"))
			imm = oprands[2].remove("0x").toInt(&ok, 16);
		if (!ok) return "";
		if (imm > 32767 || imm < -32768) return "";
		if (idx >= 33 && idx <= 36)
			imm >>= 2;
		// 将各个部分按位置入结果
		unsigned int t = 0;
		set_bit(t, op, 31, 26);
		set_bit(t, rs, 25, 21);
		set_bit(t, rt, 20, 16);
		set_bit(t, (short int)imm, 15, 0);
		// 将结果转化为字符串并返回
		QString result = QString::number(t, 16);
		result = QString(8 - result.size(), '0') + result;
		return result;
	}
	else if (type == J)
	{
		// 将操作符移除
		code.remove(OPRTR_LC[idx], Qt::CaseInsensitive);
		// 移除空格
		code.remove(' ');
		// 分离操作数
		QStringList oprands = code.split(',');
		// 如果操作数不为1个，错误返回
		if (oprands.size() != 1)
			return "";
		QString opr = oprands[0];
		// 遍历标签名，与操作数匹配
		int n = label_list.size();
		addr = -1;
		for (int i = 0; i < n; i++)
			if (opr == label_list[i].get_name())
				addr = label_list[i].get_addr();
		// 若没找到，则判断其是否为一个数字
		if (addr == -1)
		{
			bool ok;
			addr = opr.toInt(&ok, 10);
			if (!ok && opr.startsWith("0x"))
				addr = opr.remove("0x").toInt(&ok, 16);
			if (!ok) return "";
			if (addr > 0xfffffff || addr < 0) return "";
		}
		// 若不是标签也不是数字，错误返回
		if (addr == -1) return "";
		// 按位写入结果
		unsigned int t = 0;
		set_bit(t, op, 31, 26);
		set_bit(t, addr >> 2, 25, 0);
		// 将结果转化为字符串并输出
		QString result = QString::number(t, 16);
		result = QString(8 - result.size(), '0') + result;
		return result;
	}
	return "";
}