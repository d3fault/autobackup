#include <QtGui/QApplication>
#include <QFile>

#include "lastmodifieddateheirarchymolestergui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(QFile::exists("/bin/touch"))
    {
        LastModifiedDateHeirarchyMolesterGui gui;
        Q_UNUSED(gui)

        return a.exec();
    }
    qWarning("Couldn't find touch executable, quitting");
    QMetaObject::invokeMethod(QCoreApplication::instance(), "quit", Qt::QueuedConnection);
    return a.exec();
}
