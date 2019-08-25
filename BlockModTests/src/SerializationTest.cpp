#include <QApplication>
#include <QFont>
#include <QLocale>
#include <QDebug>

#include <memory>
#include <iostream>

#include <BM_Network.h>

int main(int argc, char *argv[]) {
	QCoreApplication a(argc, argv);

	// *** Locale setup for Unix/Linux ***
#if defined(Q_OS_UNIX)
	setlocale(LC_NUMERIC,"C");
#endif

	// create instance of network

	BLOCKMOD::Network network;



	// read network from file
	try {
		network.readXML("demo.net");
	}
	catch (std::exception & ex) {
		qDebug()  << ex.what();

	}

	// show contant
	network.checkNames();


	// clear network
	network = BLOCKMOD::Network();

	// add content

	// create new block at coordinates 20,50, using default appearance
	BLOCKMOD::Block b1("Block1", 20, 50);
	b1.m_size = QSize(40,100);

	// create two sockets, one inlet at left and one outlet at right within this block
	BLOCKMOD::Socket s_left("Left");
	BLOCKMOD::Socket s_right("Right");
	s_left.m_pos = QPointF(0,20);
	s_left.m_inlet = true;
	s_right.m_pos = QPointF(40+5,20);
	s_right.m_inlet = false;
	// uniquely identified by "Block1.Left"
	b1.m_sockets.append(s_left);
	// uniquely identified by "Block2.Right"
	b1.m_sockets.append(s_right); // no error checking done here, must not have two sockets with same name

	// add block to network
	network.m_blocks.append(b1);

	// create new block at coordinates 100,50, using default appearance
	BLOCKMOD::Block b2("Block2", 100, 50);
	b2.m_size = QSize(40,100);

	// create one inlet socket
	BLOCKMOD::Socket s_left2("Inlet");
	s_left2.m_pos = QPointF(0,20);
	s_left2.m_inlet = true;
	b2.m_sockets.append(s_left2);

	// add block to network, no error checking done here, must not have two blocks with same ID name
	network.m_blocks.append(b2);

	// create connector between outlet and inlet
	BLOCKMOD::Connector con;
	con.m_name = "Con1";
	con.m_sourceSocket = "Block1.Right";
	con.m_targetSocket = "Block2.Inlet";

	con.m_segments.append( BLOCKMOD::Connector::Segment(Qt::Vertical, 0));
	con.m_segments.append( BLOCKMOD::Connector::Segment(Qt::Horizontal, 0));

	network.m_connectors.append(con);

	// write network to file

	network.writeXML("demo.net");

	// return exit code to environment
	return EXIT_SUCCESS;
}
