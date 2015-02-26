#include "musicfingerscli.h"

#include <QCoreApplication>

#include "standardinputnotifier.h"
#include "../lib/musicfingers.h"

MusicFingersCli::MusicFingersCli(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
    , m_StdErr(stderr)
{
    StandardInputNotifier *sin = new StandardInputNotifier(this);
    connect(sin, SIGNAL(standardInputReceivedLine(QString)), this, SLOT(handleStandardInputLineReceived(QString)));

    MusicFingers *musicFingers = new MusicFingers(this);
    connect(musicFingers, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(musicFingers, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(musicFingers, SIGNAL(quitRequested()), this, SLOT(quit()));
    connect(this, SIGNAL(queryMusicFingerPositionRequested(int)), musicFingers, SLOT(queryMusicFingerPosition(int)));
}
void MusicFingersCli::cliUsage()
{
    handleE("invalid user input: valid values are 0-9");
}
void MusicFingersCli::handleStandardInputLineReceived(const QString &line)
{
    if(line == "q")
    {
        quit();
        return;
    }
    bool convertOk = false;
    int fingerIndexToQueryValueOf = line.toInt(&convertOk);
    if((!convertOk) || (!MusicFingers::isValidFingerId(fingerIndexToQueryValueOf)))
    {
        cliUsage();
        return;
    }
    emit queryMusicFingerPositionRequested(fingerIndexToQueryValueOf);
}
void MusicFingersCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
void MusicFingersCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void MusicFingersCli::quit()
{
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
