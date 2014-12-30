#include "qthttpservercli.h"

#include <QCoreApplication>

#include "qthttpserver.h"
#include "standardinputnotifier.h"

QtHttpServerCli::QtHttpServerCli(QObject *parent)
    : QObject(parent)
    , m_StdErr(stderr)
{
    QStringList argz = QCoreApplication::arguments();
    if(argz.length() != 2)
    {
        handleE("You didn't specify a port. Exitting");
        handleQuitRequested();
        return;
    }

    bool parseOk = false;
    quint16 port = argz.at(1).toUShort(&parseOk);
    if(!parseOk)
    {
        handleE("Invalid port. Exitting");
        handleQuitRequested();
        return;
    }

    QtHttpServer *server = new QtHttpServer(this);
    connect(server, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(server, SIGNAL(quitRequested()), this, SLOT(handleQuitRequested()));
    if(!server->startQtHttpServer(port))
    {
        handleE("failed to listen on port: " + QString::number(port));
        handleQuitRequested();
        return;
    }

    StandardInputNotifier *sin = new StandardInputNotifier(this);
    connect(sin, SIGNAL(standardInputReceivedLine(QString)), this, SLOT(handleStandardInputLineReceived(QString)));
}
void QtHttpServerCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void QtHttpServerCli::handleStandardInputLineReceived(const QString &standardInputLine)
{
    if(standardInputLine.toLower().startsWith("q"))
    {
        handleQuitRequested();
        return;
    }
}
void QtHttpServerCli::handleQuitRequested()
{
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
