#include "wasdfcli.h"

#include <QCoreApplication>

#include "wasdf.h"

WasdfCli::WasdfCli(QObject *parent)
    : QObject(parent)
    , m_StdErr(stderr)
    , m_StdOut(stdout)
{
    Wasdf *wasdf = new Wasdf(this);
    connect(wasdf, &Wasdf::e, this, &WasdfCli::handleE);
    connect(wasdf, &Wasdf::o, this, &WasdfCli::handleO);
    connect(wasdf, &Wasdf::wasdfFinished, qApp, QCoreApplication::quit);
    wasdf->startWasdf();
}
void WasdfCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void WasdfCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
