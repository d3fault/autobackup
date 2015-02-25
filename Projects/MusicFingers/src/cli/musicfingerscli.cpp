#include "musicfingerscli.h"

#include <QCoreApplication>

#include "../lib/musicfingers.h"

MusicFingersCli::MusicFingersCli(QObject *parent)
    : QObject(parent)
    , m_StdErr(stderr)
{
    MusicFingers *musicFingers = new MusicFingers(this);
    connect(musicFingers, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(musicFingers, SIGNAL(quitRequested()), qApp, SLOT(quit()), Qt::QueuedConnection);
}
void MusicFingersCli::handleE(const QString &e)
{
    m_StdErr << e << endl;
}
