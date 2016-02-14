#include <QCoreApplication>

#include "diadskipper.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	DiAdSkipper di(a.arguments());
	Q_UNUSED(di)

	return a.exec();
}
