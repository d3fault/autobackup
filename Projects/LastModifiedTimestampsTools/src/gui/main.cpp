#include <QApplication>

#include "lastmodifiedtimestampstoolsgui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LastModifiedTimestampsToolsGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
