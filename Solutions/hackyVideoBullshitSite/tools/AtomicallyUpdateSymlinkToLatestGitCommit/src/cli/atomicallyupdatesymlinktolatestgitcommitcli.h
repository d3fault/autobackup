#ifndef ATOMICALLYUPDATESYMLINKTOLATESTGITCOMMITCLI_H
#define ATOMICALLYUPDATESYMLINKTOLATESTGITCOMMITCLI_H

#include <QObject>
#include <QTextStream>

class AtomicallyUpdateSymlinkToLatestGitCommit;

class AtomicallyUpdateSymlinkToLatestGitCommitCli : public QObject
{
    Q_OBJECT
public:
    explicit AtomicallyUpdateSymlinkToLatestGitCommitCli(QObject *parent = 0);
private:
    AtomicallyUpdateSymlinkToLatestGitCommit *m_AtomicallyUpdateSymlinkToLatestGitCommit;
    QTextStream m_StdOut;
    QTextStream m_StdErr;

    void usage();
private slots:
    void handleO(const QString &msg);
    void handleE(const QString &msg);
    void handleQuitRequested();
};

#endif // ATOMICALLYUPDATESYMLINKTOLATESTGITCOMMITCLI_H
