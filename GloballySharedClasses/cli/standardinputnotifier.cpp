#include "standardinputnotifier.h"

#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
#include <QTimer>
#include <conio.h>
#else
#include <QSocketNotifier>
#include <termios.h>
#include <unistd.h>
#endif

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
    m_NotifyOnEmptyInput = false; //just pressing the enter key, for example. by default we don't even signal user input when there's nothing typed in before pressing enter
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
void StandardInputNotifier::setNotifyOnEmptyInput(bool notifyOnEmptyInput)
{
    m_NotifyOnEmptyInput = notifyOnEmptyInput;
}
void StandardInputNotifier::setEchoStandardInput(bool echoStandardInput)
{
    //NOTE: be sure to set echo standard input back to true when your app ends, because it stays off even after the app ends
#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);

    if(!echoStandardInput)
        mode &= ~ENABLE_ECHO_INPUT;
    else
        mode |= ENABLE_ECHO_INPUT;

    SetConsoleMode(hStdin, mode);
#else
    termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if(echoStandardInput)
        tty.c_lflag |= ECHO;
    else
        tty.c_lflag &= ~ECHO;
    (void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}
void StandardInputNotifier::readStandardInAndEmitSignalIfNotEmpty()
{
    QString lineOfInput = m_StandardInputTextStream->readLine(); //TODOmb: check ->device().canReadLine() first? I'm not sure 'stdin' is a regular QIODevice. I mean really though this slot only ever gets invoked when there is a line to read... but maybe on windows I should use canReadLine? Somehow I got under the impression that readLine returns an empty string if there isn't a line to read (but there are just some characters). I'm not sure that's true. Testing would be easy, but I don't have a windows box atm :-P
    if((!lineOfInput.isEmpty()) || m_NotifyOnEmptyInput)
    {
        emit standardInputReceivedLine(lineOfInput);
    }
}
