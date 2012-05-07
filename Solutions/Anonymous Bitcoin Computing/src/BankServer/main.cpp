#include <QtCore/QCoreApplication>

#include "bankserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    BankServer bankServer;
    bankServer.startListening();

    return a.exec();
}
