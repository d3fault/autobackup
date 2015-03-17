#include "nothingisknownparadoxcli.h"

#include "nothingisknownparadox.h"

NothingIsKnownParadoxCli::NothingIsKnownParadoxCli(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
{
    NothingIsKnownParadox *nothingIsKnownParadox = new NothingIsKnownParadox(this);
    connect(nothingIsKnownParadox, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
}
void NothingIsKnownParadoxCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
