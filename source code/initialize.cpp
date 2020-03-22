//========================================================================================================================
// initialize.cpp
//------------------------------------------------------------------------------------------------------------------------
// 此文件包括主程序类用于初始化的私有成员方法，主要在构造函数中调用
//========================================================================================================================

#include "myAssembler.h"

void myAssembler::set_menu()
{
	// 菜单栏 - File选项
	QMenu *file = menuBar()->addMenu(tr("&File"));
	// File - New
	QAction *new_action = new QAction(tr("New(&N)..."), this);
	// File - Open
	QMenu   *openlist = new QMenu("Open(&O)...");
	QAction *open_action_asm = new QAction(tr("Open asm(&A)..."), this);
	QAction *open_action_bin = new QAction(tr("Open bin(&B)..."), this);
	QAction *open_action_coe = new QAction(tr("Open coe(&C)..."), this);
	// File - Save
	QAction *save_action = new QAction(tr("Save(&S)..."), this);
	// File - Save as
	QMenu   *saveaslist = new QMenu("Save as(&V)...");
	QAction *saveas_action_asm = new QAction(tr("Save as asm(&A)..."), this);
	QAction *saveas_action_bin = new QAction(tr("Save as bin(&B)..."), this);
	QAction *saveas_action_coe = new QAction(tr("Save as coe(&C)..."), this);
	// File - Exit
	QAction *exit_action = new QAction(tr("Exit(&X)"), this);
	// 建立菜单项
	openlist->addAction(open_action_asm);
	openlist->addAction(open_action_bin);
	openlist->addAction(open_action_coe);
	saveaslist->addAction(saveas_action_asm);
	saveaslist->addAction(saveas_action_bin);
	saveaslist->addAction(saveas_action_coe);
	file->addAction(new_action);
	file->addMenu(openlist);
	file->addAction(save_action);
	file->addMenu(saveaslist);
	file->addAction(exit_action);
	// 设置响应
	connect(new_action, SIGNAL(triggered()), this, SLOT(newfile()));
	connect(open_action_asm, SIGNAL(triggered()), this, SLOT(open_asm()));
	connect(open_action_bin, SIGNAL(triggered()), this, SLOT(open_bin()));
	connect(open_action_coe, SIGNAL(triggered()), this, SLOT(open_coe()));
	connect(save_action, SIGNAL(triggered()), this, SLOT(save()));
	connect(saveas_action_asm, SIGNAL(triggered()), this, SLOT(save_as_asm()));
	connect(saveas_action_bin, SIGNAL(triggered()), this, SLOT(save_as_bin()));
	connect(saveas_action_coe, SIGNAL(triggered()), this, SLOT(save_as_coe()));
	connect(exit_action, &QAction::triggered, this, &myAssembler::exit);

	QMenu *build = menuBar()->addMenu(tr("&Build"));
	QAction *assemble = new QAction(tr("Assemble(&A)..."), this);
	QAction *dissemble = new QAction(tr("Dissemble(&D)..."), this);
	build->addAction(assemble);
	build->addAction(dissemble);
	connect(assemble, SIGNAL(triggered()), this, SLOT(assemble()));
	connect(dissemble, SIGNAL(triggered()), this, SLOT(dissemble()));
}

// 初始化左侧汇编代码框
void myAssembler::init_asm_text()
{
	// 设置代码框位置
	asm_text.move(25, 45);
	// 不自动换行，使编辑框可以左右滚动
	asm_text.setWordWrapMode(QTextOption::NoWrap);
	// 设置代码高亮
	Highlighter* h = new Highlighter(asm_text.document());
	// 绑定汇编框修改时的响应
	connect(&asm_text, SIGNAL(textChanged()), this, SLOT(change_asm()));
}

// 初始化右侧二进制码表
void myAssembler::init_bin_table()
{
	// 字符串流对象，用于生成地址字符串
	std::stringstream stream;
	// 设置列数及列宽
	bin_table.setColumnCount(5);
	bin_table.setColumnWidth(0, BIN_WIDTH / 2 - 20);
	bin_table.setColumnWidth(1, BIN_WIDTH / 8);
	bin_table.setColumnWidth(2, BIN_WIDTH / 8);
	bin_table.setColumnWidth(3, BIN_WIDTH / 8);
	bin_table.setColumnWidth(4, BIN_WIDTH / 8);
	// 禁止改变行列宽度
	bin_table.verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	bin_table.horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	// 隐藏行号
	bin_table.verticalHeader()->setHidden(true);
	// 设置行高
	bin_table.verticalHeader()->setDefaultSectionSize(20);
	// 设置表头字体
	QFont font("Monaco", 12);
	font.setBold(true);
	bin_table.horizontalHeader()->setFont(font);
	// 设置表头文字
	QStringList list_header;
	list_header << "Address" << "00" << "01" << "02" << "03";
	bin_table.setHorizontalHeaderLabels(list_header);
	// 设置行数
	bin_table.setRowCount(MAX_INSTRUCTIONS);
	// 设置地址值，初始化每行内容
	for (int i = 0; i < MAX_INSTRUCTIONS; i++)
	{
		stream.clear();
		stream.str("");
		stream << "0x" << std::setw(8) << std::setfill('0') << std::setiosflags(std::ios::uppercase) << std::hex << i * 4;
		QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(stream.str()));
		item->setFont(QFont("Monaco", 12));
		// 居中
		item->setTextAlignment(Qt::AlignCenter);
		// 不可修改
		item->setFlags(Qt::ItemIsEnabled);
		bin_table.setItem(i, 0, item);
	}
	// 清空表中数据
	clear_bin_table();
	// 检测输入是否合法
	connect(&bin_table, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(edit_bin_begin(QTableWidgetItem*)));
	connect(&bin_table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(edit_bin_end(QTableWidgetItem*)));
}

void myAssembler::init_filename_view()
{
	filename_asm_view.setFont(QFont("Monaco", 10));
	filename_asm_view.setText("<font color = gray>No assemble file has been opened.</font>");
	filename_boc_view.setFont(QFont("Monaco", 10));
	filename_boc_view.setText("<font color = gray>No binary or core file has been opened.</font>");
}