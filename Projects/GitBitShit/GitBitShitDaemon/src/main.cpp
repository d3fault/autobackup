#include <QCoreApplication>

#include "gitbitshitdaemon.h"

//The daemon listens for the internal meta git repo's post-update signal (however I choose to implement it). We could of course do shared nothing 'become a detached daemon if first, connect to already running otherwise', but for now I'm not even going to think about it
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    GitBitShitDaemon daemon;
    Q_UNUSED(daemon)

    return a.exec();
}
