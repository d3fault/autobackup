#include "qthttpsservercli.h"

#include <QCoreApplication>

#include "qthttpsserver.h"
#include "standardinputnotifier.h"

QtHttpsServerCli::QtHttpsServerCli(QObject *parent)
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

    QtHttpsServer *server = new QtHttpsServer(this);
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
void QtHttpsServerCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void QtHttpsServerCli::handleStandardInputLineReceived(const QString &standardInputLine)
{
    if(standardInputLine.toLower().startsWith("q"))
    {
        handleQuitRequested();
        return;
    }
}
void QtHttpsServerCli::handleQuitRequested()
{
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
