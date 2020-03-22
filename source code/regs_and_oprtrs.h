//========================================================================================================================
// regs_and_oprtrs.h
//------------------------------------------------------------------------------------------------------------------------
// 此文件包括寄存器和操作符的列表及一些常数
//========================================================================================================================

#pragma once

#include <qstring.h>

#define MAX_INSTRUCTIONS 0x1000
#define INSTRUCTION_NUM 55

const QString REGS[32] = { "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", 
						     "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
						     "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", 
						     "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra" };

const QString OPRTR_LC[INSTRUCTION_NUM] = { "lw", "lb", "lbu", "lh", "lhu", "sw", "sb", "sh", "add", "addu", "sub", "subu",
							   "slt", "sltu", "and", "or", "xor", "nor", "sll", "srl", "sra", "mult", "multu",
							   "div", "divu", "addi", "addiu", "andi", "ori", "xori", "lui", "slti", "sltiu",
							   "beq", "bne", "blez", "bgtz", "bltz", "bgez", "j", "jal", "jalr", "jr", "mfhi",
							   "mflo", "mthi", "mtlo", "eret", "mfco", "mtco", "break", "syscall", "la", "move", "li" };


const QString OPRTR_UC[INSTRUCTION_NUM] = { "LW", "LB", "LBU", "LH", "LHU", "SW", "SB", "SH", "ADD", "ADDU", "SUB", "SUBU",
							   "SLT", "SLTU", "AND", "OR", "XOR", "NOR", "SLL", "SRL", "SRA", "MULT", "MULTU",
							   "DIV", "DIVU", "ADDI", "ADDIU", "ANDI", "ORI", "XORI", "LUI", "SLTI", "SLTIU",
							   "BEQ", "BNE", "BLEZ", "BGTZ", "BLTZ", "BGEZ", "J", "JAL", "JALR", "JR", "MFHI",
							   "MFLO", "MTHI", "MTLO", "ERET", "MFCO", "MTCO", "BREAK", "SYSCALL", "LA", "MOVE", "LI" };

const unsigned int OP_OR_FUNC[INSTRUCTION_NUM] = {0x23, 0x20, 0x24, 0x21, 0x25, 0x2b, 0x28, 0x29, 0x2000, 0x2100, 0x2200, 0x2300,
										 0x2a00, 0x2b00, 0x2400, 0x2500, 0x2600, 0x2700, 0x0000, 0x0200, 0x0300, 0x1800, 0x1900,
										 0x1a00, 0x1b00, 0x08, 0x09, 0x0c, 0x0d, 0x0e, 0x0f, 0x0a, 0x0b,
										 0x04, 0x05, 0x06, 0x07, 0x8000FFFF, 0x8000FFFE, 0x02, 0x03, 0x0900, 0x0800, 0x1000,
										 0x1200, 0x1100, 0x1300, 0x1810, 0x8000FFFD, 0x8000FFFC, 0x0d00, 0x0c00, 0x8000FFFB, 0x8000FFFA, 0x8000FFF9};

const bool IS_RTYPE[INSTRUCTION_NUM] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    //25
										 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
									     1, 1, 0, 0, 0};

const bool IS_ITYPE[INSTRUCTION_NUM] = { 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    //25
										 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
										 0, 0, 0, 0, 0 };

const bool IS_JTYPE[INSTRUCTION_NUM] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    //25
										 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
										 0, 0, 0, 0, 0 };