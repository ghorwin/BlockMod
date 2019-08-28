#include <QApplication>
#include <QFont>
#include <QLocale>
#include <QtGlobal>

#include <memory>
#include <iostream>

#include "BlockModAppMainWindow.h"
#include "BlockModDemoDebugApplication.h"

void qDebugMsgHandler(QtMsgType type, const QMessageLogContext & context, const QString & msg) {
	(void) type;
	std::cout << "[" << context.function << "] " << msg.toStdString() << std::endl;
}


int main(int argc, char *argv[]) {
	BlockModAppDebugApplication a(argc, argv);

	qInstallMessageHandler(qDebugMsgHandler);

	// *** Locale setup for Unix/Linux ***
#if defined(Q_OS_UNIX)
	setlocale(LC_NUMERIC,"C");
#endif

	// *** Font size adjustment ***
#if defined(Q_OS_MAC)
	QFont f = qApp->font();
//	qDebug() << f;
	f.setPointSize(10);
	qApp->setFont(f);
	qApp->setDesktopSettingsAware(false);
#elif defined(Q_OS_UNIX)
	QFont f = qApp->font();
//	qDebug() << f;
	f.setPointSize(9);
	qApp->setFont(f);
	qApp->setDesktopSettingsAware(false);

#elif defined(Q_OS_WIN)
	QFont f = qApp->font();
//	qDebug() << f;
	f.setPointSize(8);
	qApp->setFont(f);
//	qApp->setDesktopSettingsAware(false);
#endif


	// *** Setup and show MainWindow and start event loop ***
	int res;
	try { // open scope to control lifetime of main window, ensure that main window instance dies before settings or project handler

		BlockModAppMainWindow w;

#if defined(Q_OS_WIN)
		w.showMaximized();
#elif defined(Q_OS_LINUX)
		w.show();
#else
		w.show();
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
