#include <QCoreApplication>

#include "recursivelyverifycustomdetachedgpgsignaturescli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RecursivelyVerifyCustomDetachedGpgSignaturesCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
