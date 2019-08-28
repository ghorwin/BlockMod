#include "BlockModDemoDialog.h"
#include "ui_BlockModDemoDialog.h"

#include <QGridLayout>

BlockModDemoDialog::BlockModDemoDialog(QWidget *parent) :
	QDialog(parent, Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint),
	ui(new Ui::BlockModDemoDialog)
{
	ui->setupUi(this);
	ui->gridLayout_2->setColumnStretch(0,1);
	ui->gridLayout_2->setRowStretch(1,1);
}

BlockModDemoDialog::~BlockModDemoDialog()
{
	delete ui;
}
