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
		w->setGridStep(10*BLOCKMOD::Globals::GridSpacing); // 80 pix/m; 8 pix/m for small grid

		// Create and load network
		BLOCKMOD::Network network;

		int GX = (int)BLOCKMOD::Globals::GridSpacing;

		// read network from file
#if 0
		network.readXML("demo2.net");
#else
		{
			BLOCKMOD::Block b;
			b.m_name = "Block1";
			b.m_pos = QPointF(0,0);
			b.m_size = QSize(30*GX,20*GX);

			// add an outlet socket - right
			BLOCKMOD::Socket s("T_out");
			s.m_pos = QPointF(b.m_size.width(), 2*GX); // second grid line, right side
			s.m_inlet = false;
			s.m_orientation = Qt::Horizontal;
			b.m_sockets.append(s);

			// add an outlet socket - left
			b.m_sockets.append( BLOCKMOD::Socket("T_out2", QPointF(0, 4*GX), Qt::Horizontal, false) );
			// add an outlet socket - top
			b.m_sockets.append( BLOCKMOD::Socket("T_out3", QPointF(6*GX, 0), Qt::Vertical, false) );
			// add an outlet socket - bottom
			b.m_sockets.append( BLOCKMOD::Socket("T_out4", QPointF(6*GX, b.m_size.height()), Qt::Vertical, false) );

			network.m_blocks.push_back(b);
		}
		{
			BLOCKMOD::Block b;
			b.m_name = "Block2";
			b.m_pos = QPointF(400,240);
			b.m_size = QSize(240,160);

			// add an inlet socket - left
			BLOCKMOD::Socket s("T_in", QPointF(0, 4*GX), Qt::Horizontal, true);
			b.m_sockets.append(s);

			// add an inlet socket - right
			BLOCKMOD::Socket s2("T_in2", QPointF(b.m_size.width(), 6*GX), Qt::Horizontal, true);
			b.m_sockets.append(s2);

			// add an inlet socket - top
			BLOCKMOD::Socket s3("T_in3", QPointF(4*GX, 0), Qt::Vertical, true);
			b.m_sockets.append(s3);

			// add an inlet socket - bottom
			BLOCKMOD::Socket s4("T_in4", QPointF(4*GX, b.m_size.height()), Qt::Vertical, true);
			b.m_sockets.append(s4);

			network.m_blocks.push_back(b);
		}

		// now create connectors between the various blocks and sockets

		BLOCKMOD::Connector con;
		con.m_name = "Con1";
		con.m_sourceSocket = "Block1.T_out";
		con.m_targetSocket = "Block2.T_in3";
		con.m_segments.append( BLOCKMOD::Connector::Segment(Qt::Horizontal, 0));
		con.m_segments.append( BLOCKMOD::Connector::Segment(Qt::Vertical, 0));
		network.m_connectors.push_back(con);

		con.m_name = "Con2";
		con.m_sourceSocket = "Block1.T_out4";
		con.m_targetSocket = "Block2.T_in2";
		con.m_segments.clear();
		network.m_connectors.push_back(con);

		network.adjustConnectors();
#endif

		// create scene manager (keeps network data and graphics items in sync)
		BLOCKMOD::SceneManager * scene = new BLOCKMOD::SceneManager;
		scene->setNetwork(network); // network is now known and managed by scene manager
		scene->setSceneRect(QRectF(-80,-80,800,800));
		w->setScene(scene);

		// currently, the network can be viewed, zoomed in/out and panned, and blocks can be moved

#if defined(Q_OS_WIN)
		w->showMaximized();
#elif defined(Q_OS_LINUX)
		w->show();
//		w->resize(800,600);
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
