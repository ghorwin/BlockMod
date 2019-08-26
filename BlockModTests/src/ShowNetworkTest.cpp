#include <QApplication>
#include <QFont>
#include <QLocale>
#include <QGraphicsView>
#include <QGraphicsScene>

#include <memory>
#include <iostream>

#include <BM_Network.h>
#include <BM_ZoomMeshGraphicsView.h>
#include <BM_SceneManager.h>
#include <BM_Globals.h>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	// *** Locale setup for Unix/Linux ***
#if defined(Q_OS_UNIX)
	setlocale(LC_NUMERIC,"C");
#endif

	// *** Setup and show MainWindow and start event loop ***
	int res;
	try {

		// This is our widget
		BLOCKMOD::ZoomMeshGraphicsView * w = new BLOCKMOD::ZoomMeshGraphicsView;
		w->setResolution(1); // in pix/m
		w->setGridStep(80); // 8 m = 8 pix

		// Create and load network
		BLOCKMOD::Network network;

		const int GX = (const int)BLOCKMOD::Globals::GridSpacing;

		// read network from file
#if 0
		network.readXML("demo2.net");
#else
		{
			BLOCKMOD::Block b;
			b.m_name = "Block1";
			b.m_pos = QPointF(0,0);
			b.m_size = QSize(30*GX,20*GX);

			// add an outlet socket
			BLOCKMOD::Socket s("T_out");
			s.m_pos = QPointF(b.m_size.width(), 2*GX); // second grid line, right sie
			s.m_inlet = false;
			s.m_orientation = Qt::Horizontal;
			b.m_sockets.append(s);

			network.m_blocks.append(b);
		}
		{
			BLOCKMOD::Block b;
			b.m_name = "Block2";
			b.m_pos = QPointF(400,0);
			b.m_size = QSize(240,160);

			// add an inlet socket
			BLOCKMOD::Socket s("T_out", QPointF(0, 4*GX), Qt::Horizontal, true);
			b.m_sockets.append(s);

			network.m_blocks.append(b);
		}
#endif

		// create scene manager (keeps network data and graphics items in sync)
		BLOCKMOD::SceneManager * scene = new BLOCKMOD::SceneManager;
		scene->setNetwork(network); // network is now known and managed by scene manager
		w->setScene(scene);

		// currently, the network can be viewed, zoomed in/out and panned, and blocks can be moved

#if defined(Q_OS_WIN)
		w->showMaximized();
#elif defined(Q_OS_LINUX)
		w->show();
#else
		w->show();
#endif

		// start event loop
		res = a.exec();
	} // here our mainwindow dies, main window goes out of scope and UI goes down -> destructor does ui cleanup
	catch (std::exception & ex) {
		std::cerr << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	// return exit code to environment
	return res;
}
