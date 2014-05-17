#include "atomicallyupdatesymlinktolatestgitcommitcli.h"

#include <QCoreApplication>

#include "../lib/atomicallyupdatesymlinktolatestgitcommit.h"

AtomicallyUpdateSymlinkToLatestGitCommitCli::AtomicallyUpdateSymlinkToLatestGitCommitCli(QObject *parent)
    : QObject(parent)
    , m_AtomicallyUpdateSymlinkToLatestGitCommit(new AtomicallyUpdateSymlinkToLatestGitCommit(this))
    , m_StdOut(stdout)
    , m_StdErr(stderr)
{
    QStringList args = QCoreApplication::arguments();
    if(args.size() != 4 /* || args.contains("--help", Qt::CaseInsensitive)*/) //lol fml args might actually contain "--help" if trolling
    {
        usage();
        handleQuitRequested();
        return;
    }
    connect(m_AtomicallyUpdateSymlinkToLatestGitCommit, SIGNAL(symlinkAtomicallyUpdatedToLatestGitCommit(bool)), this, SLOT(handleQuitRequested()));
    connect(m_AtomicallyUpdateSymlinkToLatestGitCommit, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(m_AtomicallyUpdateSymlinkToLatestGitCommit, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    QMetaObject::invokeMethod(m_AtomicallyUpdateSymlinkToLatestGitCommit, "atomicallyUpdateSymlinkToLatestGitCommit", Q_ARG(QString, args.at(1)), Q_ARG(QString, args.at(2)), Q_ARG(QString, args.at(3)));
}
//is usage e or o? if it's --help invoked, o. if bad args, e. for now just o fuck it
void AtomicallyUpdateSymlinkToLatestGitCommitCli::usage()
{
    QString usageStr =  "Usage: AtomicallyUpdateSymlinkToLatestGitCommitCli cloneSrcUrl symlinkToAtomicallyUpdate tempDir\n\n"

                        "\tcloneSrcUrl\t\t- The URL of the git repo to clone (with depth=1). It is recommended to specify file:// in front of a local path\n"
                        "\tsymlinkToAtomicallyUpdate\t\t- The symlink to be atomically updated (or created if non-existent). It must not be in the root directory of tempDir\n"
                        "\ttempDir\t\t- Temporary directory for both git clone destinations and symlink preparation\n";
    handleO(usageStr);
}
void AtomicallyUpdateSymlinkToLatestGitCommitCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
void AtomicallyUpdateSymlinkToLatestGitCommitCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void AtomicallyUpdateSymlinkToLatestGitCommitCli::handleQuitRequested()
{
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
