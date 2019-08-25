#include "BlockModAppDebugApplication.h"

#include <iostream>

bool BlockModAppDebugApplication::notify( QObject *recv, QEvent *e ) {

	try {
		return QApplication::notify( recv, e );
	}
	catch (std::exception &ex) {
		std::cerr << ex.what() << std::cerr;
	}

	return false;
}

