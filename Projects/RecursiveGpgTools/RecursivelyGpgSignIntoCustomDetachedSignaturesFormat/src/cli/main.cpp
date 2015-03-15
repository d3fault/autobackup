#include <QCoreApplication>

#include "recursivelygpgsignintocustomdetachedsignaturesformatcli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
