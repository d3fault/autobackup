#include <QApplication>

#include "../lib/objectssynchronizer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ObjectsSynchronizer s(&a, false);
    s.addAndConstructAndInitializeAndStartObject<QObject>("lol");

    return a.exec();
}
