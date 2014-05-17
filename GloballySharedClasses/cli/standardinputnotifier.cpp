#include "standardinputnotifier.h"

//because windows is teh suck, but i still really want to use qsocketnotifier wherever i can
StandardInputNotifier::StandardInputNotifier(QObject *parent)
    : QObject(parent)
{
    constructor(500);
}
StandardInputNotifier::StandardInputNotifier(int msecTimeoutForPollingStandardInput_WINDOWS_ONLY, QObject *parent)
    : QObject(parent)
{
    constructor(msecTimeoutForPollingStandardInput_WINDOWS_ONLY);
}
StandardInputNotifier::~StandardInputNotifier()
{
    delete m_StandardInputTextStream;
}
void StandardInputNotifier::constructor(int msecTimeoutForPollingStandardInput_WINDOWS_ONLY)
{
    m_StandardInputTextStream = new QTextStream(stdin, QIODevice::ReadOnly);
#if defined(Q_OS_WIN) || defined(Q_WS_WIN) //targetting 4 && 5 is becoming a bitch xD, i'm thinking of just doing 5 since it's inevitibru, but i also don't have any compelling reason to switch. even if i did, i'd likely keep it on a project by project basis
    m_StandardInputPollingTimerLoLWindows = new QTimer(this);
    connect(m_StandardInputPollingTimerLoLWindows, SIGNAL(timeout()), this, SLOT(readStandardInAndEmitSignalIfNotEmpty()));
    m_StandardInputPollingTimerLoLWindows->start(msecTimeoutForPollingStandardInput_WINDOWS_ONLY);
#else
    Q_UNUSED(msecTimeoutForPollingStandardInput_WINDOWS_ONLY)
    m_StandardInputSocketNotifierWootEventDriven = new QSocketNotifier(0/*portable? doesn't need to be portable to windows*/, QSocketNotifier::Read, this);
    connect(m_StandardInputSocketNotifierWootEventDriven, SIGNAL(activated(int)), this, SLOT(readStandardInAndEmitSignalIfNotEmpty()));
#endif
}
void StandardInputNotifier::readStandardInAndEmitSignalIfNotEmpty()
{
    QString lineOfInput = m_StandardInputTextStream->readLine();
    if(!lineOfInput.isEmpty())
    {
        emit standardInputReceivedLine(lineOfInput);
    }
}