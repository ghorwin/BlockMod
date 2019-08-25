#include "BlockModAppMainWindow.h"
#include "ui_BlockModAppMainWindow.h"

#include <QGraphicsScene>
#include <QGraphicsItem>

BlockModAppMainWindow::BlockModAppMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::BlockModAppMainWindow),
	m_scene(nullptr)
{
	ui->setupUi(this);
}


BlockModAppMainWindow::~BlockModAppMainWindow() {
	delete ui;
}


void BlockModAppMainWindow::on_actionFileNew_triggered() {
	// create a new network
	BLOCKMOD::Network network;
}


void BlockModAppMainWindow::on_actionFileQuit_triggered() {
	close();
}


void BlockModAppMainWindow::setNetwork(const BLOCKMOD::Network & network) {
	// disconnect old representation from network (delete and recreate graphics scene and all items)
	if (m_scene != nullptr) {
		QList<QGraphicsItem*> items = m_scene->items();
		qDeleteAll(items);
		delete m_scene; m_scene = nullptr;
	}

	// create new graphics scene
	m_scene  = new BLOCKMOD::SceneManager(this);
	ui->graphicsView->setScene(m_scene);

	m_network = network;
	m_scene->setNetwork(m_network);
}

