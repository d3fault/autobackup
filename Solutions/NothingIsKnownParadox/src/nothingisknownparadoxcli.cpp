#include "nothingisknownparadoxcli.h"

#include <QSocketNotifier>

#include "nothingisknownparadox.h"

NothingIsKnownParadoxCli::NothingIsKnownParadoxCli(QObject *parent)
    : QObject(parent)
    , m_StdIn(stdin)
    , m_StdOut(stdout)
{
    NothingIsKnownParadox *nothingIsKnownParadox = new NothingIsKnownParadox(this);
    connect(nothingIsKnownParadox, SIGNAL(o(QString)), this, SLOT(handleO(QString)));

    QSocketNotifier *stdInSocketNotifier = new QSocketNotifier(0, QSocketNotifier::Read, this);
    connect(stdInSocketNotifier, SIGNAL(activated(int)), this, SIGNAL(quitRequested())); //quit on any user input
}
void NothingIsKnownParadoxCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
