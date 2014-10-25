#include <QCoreApplication>

#include "notifygitbitshitdaemonofmessagecli.h"

//post-udpate calls this, which tells the daemon to re-examine the meta file it's watching because it changed. it now contains the info hash of the latest git commit
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    NotifyGitBitShitDaemonOfNotifyGitBitShitDaemonOfMessageCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
