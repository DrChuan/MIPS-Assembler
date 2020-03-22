# MIPS汇编器

### 一、项目说明
本项目实现的是MIPS体系结构下的汇编和反汇编功能。开发环境是Microsoft Visual Studio 2017, 使用Qt5.13作为开发框架，采用C++编程，实现了简单的用户界面。

source code目录下共16个文件，包括11个cpp源文件，5个头文件。分别是：
- asm_text_highlight.cpp
- asm_to_bin.cpp
- file_IO.cpp
- initialize.cpp
- instruction.cpp
- main.cpp
- myAssembler.cpp
- processor.cpp
- segment.cpp
- slots.cpp
- window_inputcount.cpp

以及：
- asm_text_highlight.h
- myAssembler.h
- processor.h
- regs_and_oprts.h
- window_inputcount.h

### 二、功能简介
本项目由于开发时间较短，只实现了十分有限的功能。简单介绍如下：

1. 打开文件：支持asm文件，bin文件，coe文件的加载。
2. 保存文件：支持asm文件，bin文件，coe文件的保存。保存为bin或coe时会询问要保存的字节数。
3. 汇编：将程序框中的汇编代码汇编成MIPS架构下的机器指令。支持基本的R型、I型算术逻辑指令，跳转指令，读写指令，R型和J型跳转指令，同时支持利用DataAddre:和BaseAddre:指定数据段和代码段的地址，在数据段中用伪指令db(define byte), dw(define word), dd(define double word)定义数据。各段前后不得有重叠的地址，否则将出错。
4. 反汇编：将地址窗中的机器码反汇编成汇编指令。反汇编的范围会在执行前询问用户。

本项目还有一些预期功能没有实现，笔者可能在后期继续添加。
