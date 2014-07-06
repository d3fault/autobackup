#include <QCoreApplication>

#include "zop.h"

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    Zop z;
    QString yoloStr("yolo");
    QMetaObject::invokeMethod(&z, "zopSlot", Q_ARG(QString, yoloStr));

    return a.exec();
}
