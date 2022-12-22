#include "BlockModDemoDialog.h"
#include "ui_BlockModDemoDialog.h"

#include <stdexcept>

#include <QGridLayout>
#include <QFileDialog>
#include <QDebug>

#include <BM_SceneManager.h>
#include <BM_Network.h>
#include <BM_BlockItem.h>
#include <BM_ConnectorSegmentItem.h>
#include <BM_Globals.h>

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
		std::list<BLOCKMOD::Connector> checkedCons;
		// remove invalid connections and fix any connectors that might miss a bit
		for (BLOCKMOD::Connector & con : n.m_connectors) {
			try {
				n.adjustConnector(con);
				checkedCons.push_back(con);
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


void BlockModDemoDialog::on_pushButtonRemoveBlock_clicked() {
	// find out selected blocks

	QList<const BLOCKMOD::Block *> selectedBlocks = m_sceneManager->selectedBlocks();
	for (const BLOCKMOD::Block * bi : selectedBlocks) {
		m_sceneManager->removeBlock(bi);
	}
}


void BlockModDemoDialog::on_pushButtonRemoveConnection_clicked() {
	// no connector selected?
	const BLOCKMOD::Connector * con = m_sceneManager->selectedConnector();
	if (con == nullptr)
		return;
	m_sceneManager->removeConnector(con);
}


void BlockModDemoDialog::on_pushButtonAddBlock_clicked() {
	// generate a random block and add it to the scene
	BLOCKMOD::Block b;
	int len = 4+int(qrand()*4.0/RAND_MAX);
	for (int i=0; i<len; ++i)
		b.m_name.append('a'+int(qrand()*26.0/RAND_MAX));

	int gridX = len+4+int(qrand()*8.0/RAND_MAX);
	int gridY = 3+int(qrand()*8.0/RAND_MAX);

	b.m_size = QSizeF(gridX*BLOCKMOD::Globals::GridSpacing, gridY*BLOCKMOD::Globals::GridSpacing);
	b.m_pos = QPointF(int(qrand()*30.0/RAND_MAX)*BLOCKMOD::Globals::GridSpacing, int(qrand()*30.0/RAND_MAX)*BLOCKMOD::Globals::GridSpacing);


	// create and position sockets
	for (int i=0; i<gridX-2; i+=2) {
		bool haveSocket = qrand()*6.0/RAND_MAX < 1;
		if (haveSocket) {
			BLOCKMOD::Socket s;
			s.m_inlet = qrand()*2.0/RAND_MAX < 1;
			int slen = int(qrand()*6.0/RAND_MAX);
			for (int i=0; i<slen; ++i)
				s.m_name.append('a'+int(qrand()*26.0/RAND_MAX));
			s.m_orientation = Qt::Vertical;
			s.m_pos = QPointF((i+1)*BLOCKMOD::Globals::GridSpacing, 0);
			b.m_sockets.append(s);
		}
		haveSocket = qrand()*6.0/RAND_MAX < 1;
		if (haveSocket) {
			BLOCKMOD::Socket s;
			s.m_inlet = qrand()*2.0/RAND_MAX < 1;
			int slen = int(qrand()*6.0/RAND_MAX);
			for (int i=0; i<slen; ++i)
				s.m_name.append('a'+int(qrand()*26.0/RAND_MAX));
			s.m_orientation = Qt::Vertical;
			s.m_pos = QPointF((i+1)*BLOCKMOD::Globals::GridSpacing, b.m_size.height());
			b.m_sockets.append(s);
		}
	}

	m_sceneManager->addBlock(b);
}
