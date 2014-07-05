#include <QCoreApplication>

#include "zop.h"

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    Zop z;
    Q_UNUSED(z)

    return a.exec();
}
