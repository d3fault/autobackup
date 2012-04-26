#include <QtCore/QCoreApplication>

#include "abcappdb.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    AbcAppDb appDb;
    appDb.connectToBankServerAndStartListeningForWtFrontEnds(); //one SSL client connection to the bank server, multiple incoming SSL connections from Wt Front Ends

    return a.exec();
}
