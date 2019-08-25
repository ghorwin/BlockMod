#ifndef BLOCKMODAPPMAINWINDOW_H
#define BLOCKMODAPPMAINWINDOW_H

#include <QMainWindow>

#include <BM_Network.h>
#include <BM_SceneManager.h>

class QAbstractGraphicsItems;

namespace Ui {
	class BlockModAppMainWindow;
}

/*! Test application for the block modeling toolkit library.
	Feel free to use this as foundation for your own application.


*/
class BlockModAppMainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit BlockModAppMainWindow(QWidget *parent = 0);
	~BlockModAppMainWindow();

private slots:
	void on_actionFileNew_triggered();
	void on_actionFileQuit_triggered();

private:
	/*! Replaces an existing network/graphics scene.
		Creates all graphics items to draw this network, replaces old graphics scene with new graphics scene.
	*/
	void setNetwork(const BLOCKMOD::Network & network);

	Ui::BlockModAppMainWindow	*ui;

	/*! Our data storage member. */
	BLOCKMOD::Network			m_network;

	/*! Currently our only representation of a single network. */
	BLOCKMOD::SceneManager		*m_scene;
};

#endif // BLOCKMODAPPMAINWINDOW_H
