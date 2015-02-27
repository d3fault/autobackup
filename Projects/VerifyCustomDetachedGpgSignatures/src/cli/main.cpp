#include <QCoreApplication>

#include "verifycustomdetachedgpgsignaturescli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    VerifyCustomDetachedGpgSignaturesCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
