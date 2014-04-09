#include "standardinputnotifier.h"

//because windows is teh suck, but i still really want to use qsocketnotifier wherever i can
StandardInputNotifier::StandardInputNotifier(QObject *parent)
{
    StandardInputNotifier(200, parent);
}
StandardInputNotifier::StandardInputNotifier(int msecTimeoutForPollingStandardInput_WINDOWS_ONLY, QObject *parent) :
    QObject(parent), m_StandardInputTextStream(stdin, QIODevice::ReadOnly)
  #if defined(Q_OS_WIN32) || defined(Q_OS_WINCE)
    , m_StandardInputPollingTimerLoLWindows(new QTimer(this))
  #else
    , m_StandardInputSocketNotifierWootEventDriven(new QSocketNotifier(0/*portable?*/, QSocketNotifier::Read, this))
  #endif
{
#if defined(Q_OS_WIN32) || defined(Q_OS_WINCE)
    connect(m_StandardInputPollingTimerLoLWindows, SIGNAL(timeout()), this, SLOT(readStandardInAndEmitSignalIfNotEmpty()));
    m_StandardInputPollingTimerLoLWindows->start(msecTimeoutForPollingStandardInput_WINDOWS_ONLY);
#else
    Q_UNUSED(msecTimeoutForPollingStandardInput_WINDOWS_ONLY)
    connect(m_StandardInputSocketNotifierWootEventDriven, SIGNAL(activated(int)), this, SLOT(readStandardInAndEmitSignalIfNotEmpty()));
#endif
}
void StandardInputNotifier::readStandardInAndEmitSignalIfNotEmpty()
{
    QString lineOfInput = m_StandardInputTextStream.readLine();
    if(!lineOfInput.isEmpty())
    {
        emit standardInputReceivedLine(lineOfInput);
    }
}
