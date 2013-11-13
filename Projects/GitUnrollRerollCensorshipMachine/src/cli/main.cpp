#include <QtCore/QCoreApplication>

#include "gitunrollrerollcensorshipmachinecli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    GitUnrollRerollCensorshipMachineCli cli;
    QMetaObject::invokeMethod(&cli, "parseArgsAndThenDoGitUnrollRerollCensoring", Qt::QueuedConnection);

    return a.exec();
}
