#include "quickdirtystaticgitwebcli.h"

#include <QCoreApplication>

#include "quickdirtystaticgitweb.h"

QuickDirtyStaticGitWebCli::QuickDirtyStaticGitWebCli(QObject *parent)
    : QObject(parent)
    , m_StdErr(stderr)
{
    QuickDirtyStaticGitWeb *quickDirtyStaticGitWeb = new QuickDirtyStaticGitWeb(this);

    connect(quickDirtyStaticGitWeb, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(quickDirtyStaticGitWeb, SIGNAL(finishedGeneratingStaticGitWeb(bool)), this, SLOT(handleFinishedGeneratingStaticGitWeb(bool)), Qt::QueuedConnection); //queued so the event loop will have been entered and our call to qApp->exit(int) isn't ignored!

    QStringList argz = qApp->arguments();
    argz.takeFirst(); //app file path
    if(argz.size() != 2)
    {
        usage();
        QMetaObject::invokeMethod(this, "handleFinishedGeneratingStaticGitWeb", Qt::QueuedConnection, Q_ARG(bool,false));
        return;
    }
    QString sourceInputRepo = argz.at(0);
    QString destinationOutputStaticGitWebRepo = argz.at(1);
    quickDirtyStaticGitWeb->generateStaticGitWeb(sourceInputRepo, destinationOutputStaticGitWebRepo);
}
void QuickDirtyStaticGitWebCli::usage()
{
    //TODOreq
    handleE("incorrect usage");
}
void QuickDirtyStaticGitWebCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void QuickDirtyStaticGitWebCli::handleFinishedGeneratingStaticGitWeb(bool success)
{
    qApp->exit(success ? 0 : 1);
}
