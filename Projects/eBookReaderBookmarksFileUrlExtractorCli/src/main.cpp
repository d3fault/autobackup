#include <QCoreApplication>

#include "ebookreaderbookmarksfileurlextractorcli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    eBookReaderBookmarksFileUrlExtractorCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
