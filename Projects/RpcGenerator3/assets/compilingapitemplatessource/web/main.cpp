#include <QCoreApplication>

#include "cleanroomweb.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CleanRoomWeb web(argc, argv);
    Q_UNUSED(web)

    return a.exec();
}
