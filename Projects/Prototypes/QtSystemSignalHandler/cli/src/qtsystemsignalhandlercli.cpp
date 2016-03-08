#include "qtsystemsignalhandlercli.h"

#include <QCoreApplication>

#include "qtsystemsignalhandler.h"

QtSystemSignalHandlerCli::QtSystemSignalHandlerCli(QObject *parent)
    : QObject(parent)
{
    connect(QtSystemSignalHandler::instance(), SIGNAL(systemSignalReceived(QtSystemSignal::QtSystemSignalEnum)), this, SLOT(handleSigTermOrSigIntReceived()));
}
void QtSystemSignalHandlerCli::handleSigTermOrSigIntReceived()
{
    qDebug("SIGINT or SIGTERM received!");
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
