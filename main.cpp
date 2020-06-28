#include "mainwindow.h"
#include "welcomewindow.h"
#include <QApplication>

int main(int argc, char *argv[])//基本没变
{
	QApplication a(argc, argv);
    WelcomeWindow w;
	w.show();

	return a.exec();
}
