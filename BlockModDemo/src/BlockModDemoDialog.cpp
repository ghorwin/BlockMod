#include "BlockModDemoDialog.h"
#include "ui_BlockModDemoDialog.h"

#include <stdexcept>

#include <QGridLayout>
#include <QFileDialog>
#include <QDebug>

#include <BM_SceneManager.h>
#include <BM_Network.h>
#include <BM_BlockItem.h>

BlockModDemoDialog::BlockModDemoDialog(QWidget *parent) :
	QDialog(parent, Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint),
	ui(new Ui::BlockModDemoDialog),
	m_sceneManager(new BLOCKMOD::SceneManager(this))
{
	ui->setupUi(this);
	ui->gridLayout_2->setColumnStretch(0,1);
	ui->gridLayout_2->setRowStretch(1,1);

	// set scene to network graphics view
	ui->graphicsView->setScene(m_sceneManager);
	ui->graphicsView->setResolution(1); // in pix/m
	ui->graphicsView->setGridStep(80); // 80 pix/m; 8 pix/m for small grid

	loadNetwork("demo.bm");
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
	loadNetwork(fname);
}


void BlockModDemoDialog::on_toolButtonSave_clicked() {
	QString fname = QFileDialog::getSaveFileName(this, tr("Select BlockMod file"), QString(), tr("BlockMod files (*.bm)"));
	if (fname.isEmpty())
		return;
	m_sceneManager->network().writeXML(fname);
}


void BlockModDemoDialog::loadNetwork(const QString & fname) {
	BLOCKMOD::Network n;
	try {
		n.readXML(fname);
		n.checkNames();
		QList<BLOCKMOD::Connector> checkedCons;
		// remove invalid connections and fix any connectors that might miss a bit
		for (BLOCKMOD::Connector & con : n.m_connectors) {
			try {
				n.adjustConnector(con);
				checkedCons.append(con);
			}
			catch (...) {}
		}
		n.m_connectors.swap(checkedCons);
		m_sceneManager->setNetwork(n); // network is copied
	} catch (std::runtime_error & e) {
		QString errormsg(e.what());
		qDebug() << errormsg;
	}
}


void BlockModDemoDialog::on_pushButtonConnectSockets_clicked() {
	// put scene into connection mode by turning on outlet socket hovering
	m_sceneManager->enableConnectionMode();
}

void BlockModDemoDialog::on_pushButtonRemoveBlock_clicked() {
	// find out selected blocks

	QList<QGraphicsItem*> selectedItems = m_sceneManager->selectedItems();
	QList<BLOCKMOD::BlockItem *> selectedBlocks;
	for (QGraphicsItem * item : selectedItems) {
		BLOCKMOD::BlockItem * bi = dynamic_cast<BLOCKMOD::BlockItem *>(item);
		if (bi != nullptr)
			selectedBlocks.append(bi);
	}

	for (BLOCKMOD::BlockItem * bi : selectedBlocks) {
		for (int idx = 0; idx < m_sceneManager->network().m_blocks.count(); ++idx) {
			if (&m_sceneManager->network().m_blocks[idx] == bi->block()) {
				m_sceneManager->removeBlock(idx);
			}
		}
	}
}
