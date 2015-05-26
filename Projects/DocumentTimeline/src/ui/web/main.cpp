#include <Wt/WServer>
#include <QCoreApplication>

#include "documenttimelineweb.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DocumentTimelineWeb documentTimelineWeb(argc, argv);
    Q_UNUSED(documentTimelineWeb)

    return a.exec();
}
