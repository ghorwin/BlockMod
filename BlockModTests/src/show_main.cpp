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
		w->setGridStep(0.05);

		// Create and load network
		BLOCKMOD::Network network;

		// read network from file
		network.readXML("demo2.net");

		// create scene manager (keeps network data and graphics items in sync)
		BLOCKMOD::SceneManager * scene = new BLOCKMOD::SceneManager;
		scene->setNetwork(network); // network is now known and managed by scene manager
		w->setScene(scene);

		// currently, the network can be viewed, zoomed in/out and panned, nothing else

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
