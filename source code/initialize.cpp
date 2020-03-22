//========================================================================================================================
// initialize.cpp
//------------------------------------------------------------------------------------------------------------------------
// ���ļ����������������ڳ�ʼ����˽�г�Ա��������Ҫ�ڹ��캯���е���
//========================================================================================================================

#include "myAssembler.h"

void myAssembler::set_menu()
{
	// �˵��� - Fileѡ��
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
	// �����˵���
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
	// ������Ӧ
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

// ��ʼ�����������
void myAssembler::init_asm_text()
{
	// ���ô����λ��
	asm_text.move(25, 45);
	// ���Զ����У�ʹ�༭��������ҹ���
	asm_text.setWordWrapMode(QTextOption::NoWrap);
	// ���ô������
	Highlighter* h = new Highlighter(asm_text.document());
	// �󶨻����޸�ʱ����Ӧ
	connect(&asm_text, SIGNAL(textChanged()), this, SLOT(change_asm()));
}

// ��ʼ���Ҳ���������
void myAssembler::init_bin_table()
{
	// �ַ����������������ɵ�ַ�ַ���
	std::stringstream stream;
	// �����������п�
	bin_table.setColumnCount(5);
	bin_table.setColumnWidth(0, BIN_WIDTH / 2 - 20);
	bin_table.setColumnWidth(1, BIN_WIDTH / 8);
	bin_table.setColumnWidth(2, BIN_WIDTH / 8);
	bin_table.setColumnWidth(3, BIN_WIDTH / 8);
	bin_table.setColumnWidth(4, BIN_WIDTH / 8);
	// ��ֹ�ı����п��
	bin_table.verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	bin_table.horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	// �����к�
	bin_table.verticalHeader()->setHidden(true);
	// �����и�
	bin_table.verticalHeader()->setDefaultSectionSize(20);
	// ���ñ�ͷ����
	QFont font("Monaco", 12);
	font.setBold(true);
	bin_table.horizontalHeader()->setFont(font);
	// ���ñ�ͷ����
	QStringList list_header;
	list_header << "Address" << "00" << "01" << "02" << "03";
	bin_table.setHorizontalHeaderLabels(list_header);
	// ��������
	bin_table.setRowCount(MAX_INSTRUCTIONS);
	// ���õ�ֵַ����ʼ��ÿ������
	for (int i = 0; i < MAX_INSTRUCTIONS; i++)
	{
		stream.clear();
		stream.str("");
		stream << "0x" << std::setw(8) << std::setfill('0') << std::setiosflags(std::ios::uppercase) << std::hex << i * 4;
		QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(stream.str()));
		item->setFont(QFont("Monaco", 12));
		// ����
		item->setTextAlignment(Qt::AlignCenter);
		// �����޸�
		item->setFlags(Qt::ItemIsEnabled);
		bin_table.setItem(i, 0, item);
	}
	// ��ձ�������
	clear_bin_table();
	// ��������Ƿ�Ϸ�
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