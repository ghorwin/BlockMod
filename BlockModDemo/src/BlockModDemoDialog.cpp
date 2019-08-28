#include "BlockModDemoDialog.h"
#include "ui_BlockModDemoDialog.h"

BlockModDemoDialog::BlockModDemoDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::BlockModDemoDialog)
{
	ui->setupUi(this);
}

BlockModDemoDialog::~BlockModDemoDialog()
{
	delete ui;
}
