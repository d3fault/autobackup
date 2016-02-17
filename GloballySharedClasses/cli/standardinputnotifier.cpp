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
char StandardInputNotifier::getch()
{
    //NOTE: this method should only be used after calling setDontWaitForEnterToBePressedAndEmitEveryCharacterIndividually(true);
    char buf;
    if(read(STDIN_FILENO, &buf, 1) < 1)
        qFatal("Read less than 1 bytes");
    return buf;
}
StandardInputNotifier::~StandardInputNotifier()
{
    delete m_StandardInputTextStream;
}
void StandardInputNotifier::populateOriginalAndCurrentTtyIfNeeded()
{
    if(!m_OriginalTty.isNull())
        return;
    m_OriginalTty.reset(new termios());
    tcgetattr(STDIN_FILENO, m_OriginalTty.data());
    m_CurrentTty = *(m_OriginalTty.data());
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
    populateOriginalAndCurrentTtyIfNeeded();
    if(echoStandardInput)
        m_CurrentTty.c_lflag |= ECHO;
    else
        m_CurrentTty.c_lflag &= ~ECHO;
    (void)tcsetattr(STDIN_FILENO, TCSANOW, &m_CurrentTty);

#if 0 //TODOreq
#ifdef Q_OS_WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);

    if(!enabled)
        mode &= ~ENABLE_ECHO_INPUT;
    else
        mode |= ENABLE_ECHO_INPUT;

    SetConsoleMode(hStdin, mode);

#else
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);

    if(!enabled)
    {
        tty.c_lflag &= ~ECHO;
        tty.c_lflag &= ~ICANON;
        tty.c_cc[VMIN] = 1;
        tty.c_cc[VTIME] = 0;
    }
    else
    {
        tty.c_lflag |= ECHO;
        tty.c_lflag |= ICANON;
    }

    (void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
#endif
}
void StandardInputNotifier::setDontWaitForEnterToBePressedAndEmitEveryCharacterIndividually(bool dontWaitForEnterToBePressedAndEmitEveryCharacterIndividually)
{
    //note, although this method does set up the terminal, you no longer listen to standardInputReceivedLine and instead call the blocking getch method
    populateOriginalAndCurrentTtyIfNeeded();
    if(dontWaitForEnterToBePressedAndEmitEveryCharacterIndividually)
    {
        disconnect(m_StandardInputSocketNotifierWootEventDriven, SIGNAL(activated(int)));
        m_CurrentTty.c_lflag &= ~ICANON;
        m_CurrentTty.c_cc[VMIN] = 1;
        m_CurrentTty.c_cc[VTIME] = 0;
    }
    else
    {
        connect(m_StandardInputSocketNotifierWootEventDriven, SIGNAL(activated(int)), this, SLOT(readStandardInAndEmitSignalIfNotEmpty()));
        m_CurrentTty.c_lflag |= ICANON;
    }
    (void)tcsetattr(STDIN_FILENO, TCSANOW, &m_CurrentTty);
}
void StandardInputNotifier::readStandardInAndEmitSignalIfNotEmpty()
{
    QString lineOfInput = m_StandardInputTextStream->readLine(); //TODOmb: check ->device().canReadLine() first? I'm not sure 'stdin' is a regular QIODevice. I mean really though this slot only ever gets invoked when there is a line to read... but maybe on windows I should use canReadLine? Somehow I got under the impression that readLine returns an empty string if there isn't a line to read (but there are just some characters). I'm not sure that's true. Testing would be easy, but I don't have a windows box atm :-P
    if((!lineOfInput.isEmpty()) || m_NotifyOnEmptyInput)
    {
        emit standardInputReceivedLine(lineOfInput);
    }
}
