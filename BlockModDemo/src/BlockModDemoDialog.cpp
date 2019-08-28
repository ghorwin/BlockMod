#include "BlockModDemoDialog.h"
#include "ui_BlockModDemoDialog.h"

#include <stdexcept>

#include <QGridLayout>
#include <QFileDialog>

#include <BM_SceneManager.h>
#include <BM_Network.h>

BlockModDemoDialog::BlockModDemoDialog(QWidget *parent) :
	QDialog(parent, Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint),
	ui(new Ui::BlockModDemoDialog),
	m_sceneManager(new BLOCKMOD::SceneManager(this))
{
	ui->setupUi(this);
	ui->gridLayout_2->setColumnStretch(0,1);
	ui->gridLayout_2->setRowStretch(1,1);
}


BlockModDemoDialog::~BlockModDemoDialog() {
	delete ui;
}


void BlockModDemoDialog::on_toolButtonNew_clicked() {
	BLOCKMOD::Network n;
	m_sceneManager->setNetwork(n); // network is copied
}


void BlockModDemoDialog::on_toolButtonOpen_clicked() {
	QString fname = QFileDialog::getOpenFileName(this, tr("Select BlockMod file"), QString(), tr("BlockMod files (*.bm)"));
	if (fname.isEmpty())
		return;

	BLOCKMOD::Network n;
	try {
		n.readXML(fname);
	} catch (std::runtime_error & e) {
		QString errormsg(e.what());
		ui->plainTextEdit->appendPlainText(errormsg + '\n');
	}
}
