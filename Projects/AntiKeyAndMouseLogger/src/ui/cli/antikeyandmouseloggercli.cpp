#include "antikeyandmouseloggercli.h"

#include <QCoreApplication>

#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
#include <conio.h> //might need more includes, maybe unistd?
#else
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#endif

#include "standardinputnotifier.h"
#include "qtsystemsignalhandler.h"

AntiKeyAndMouseLoggerCli::AntiKeyAndMouseLoggerCli(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
{
    QtSystemSignalHandler *systemSignalHandler = new QtSystemSignalHandler(this);
    connect(systemSignalHandler, SIGNAL(systemSignalReceived(QtSystemSignal::QtSystemSignalEnum)), qApp, SLOT(quit())); //to get the destructor to run when Ctrl+C / `kill <pid>` occurs... because if the destructor doesn't run, the terminal is left with canonical mode off and stdin echo'ing disabled

    AntiKeyAndMouseLogger *anti = new AntiKeyAndMouseLogger(this);
    connect(anti, SIGNAL(presentShuffledKeymapPageRequested(KeyMap)), this, SLOT(presentShuffledKeymapPage(KeyMap)));
    connect(this, SIGNAL(translateShuffledKeymapEntryRequested(QString)), anti, SLOT(translateShuffledKeymapEntry(QString)));
    connect(anti, SIGNAL(shuffledKeymapEntryTranslated(KeyMapEntry)), this, SLOT(handleShuffledKeymapEntryTranslated(KeyMapEntry)));

    StandardInputNotifier::setEchoStandardInput(false);
    set_GetCh_Hackery(true);

    anti->generateShuffledKeymapAndRequestPresentationOfFirstPage();
}
AntiKeyAndMouseLoggerCli::~AntiKeyAndMouseLoggerCli()
{
    m_StdOut << endl << tr("Password: ") << m_Password << endl; //TODOreq: ask them what to do with password. echo it, write it to a file, etc
    set_GetCh_Hackery(false);
    StandardInputNotifier::setEchoStandardInput(true);
}
void AntiKeyAndMouseLoggerCli::set_GetCh_Hackery(bool enable_GetCh_Hackery)
{
    //NOTE: be sure to set getch hackery back to false when your app ends, because it stays on even after the app ends
#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
    //noop? there might be some hackery needed to be done here, idk
#else
    termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if(enable_GetCh_Hackery)
    {
        tty.c_lflag &= ~ICANON;
        tty.c_cc[VMIN] = 1;
        tty.c_cc[VTIME] = 0;
    }
    else
        tty.c_lflag |= ICANON;
    (void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}
char AntiKeyAndMouseLoggerCli::myGetCh()
{
#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
    return _getch(); //TODOwin32: what happens when SIGINT or that SIGWINDOWCLOSED-thing is received?
#else
    char buf;
    ssize_t sizeRead = read(STDIN_FILENO, &buf, 1);
    bool shouldQuit = false;
    if(sizeRead == 0 /*eof*/)
        shouldQuit = true;
    else if(sizeRead > 1 /*wtf error should never happen)*/)
    {
        m_StdOut << "Error: we read() more than 1 character!";
        shouldQuit = true;
    }
    if(sizeRead < 0)
    {
        if(errno == EINTR)
        {
            //SIGINT or SIGTERM received while we were reading. in theory this could have been a USR1 or USR2 etc in which case i shouldn't set shouldQuit to true. but since I know it's not those, I'm going to set shouldQuit to true just to KISS (even though or system signal handler already (or is about to) dispatch the "quit" signal! double "quit"ing is fine, I just want to simplify this 'stop reading' code path)
            shouldQuit = true;
        }
        else
        {
            m_StdOut << "Error: read() returned < 0";
            shouldQuit = true;
        }
    }
    else
    {
        //sizeRead == 1
    }
    if(shouldQuit)
    {
        return static_cast<char>(static_cast<int>(10)); //return an 'Enter', so it's as if they chose to quit by pressing Enter
    }
    return buf;
#endif
}
void AntiKeyAndMouseLoggerCli::presentShuffledKeymapPage(const KeyMap &shuffledKeymapPage)
{
    m_StdOut << endl << endl;
    int column = 1;
    m_StdOut << "\t" << AntiKeyAndMouseLogger::legend() << endl;
    Q_FOREACH(const KeymapHashTypes &currentEntry, shuffledKeymapPage)
    {
        //TODOmb: sum ascii art gridz
        m_StdOut << currentEntry.first << " = " << currentEntry.second;
        if(++column > AntiKeyAndMouseLogger_COLUMNS_PER_ROW)
        {
            column = 1;
            m_StdOut << endl;
        }
        else
            m_StdOut << "   ";
    }
    m_StdOut << endl << tr("Type selection (or press 'Enter' to end program): ");
    m_StdOut.flush();

    char keyPressed = myGetCh();
    if(static_cast<int>(keyPressed) == 10)
    {
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }
    QString charStr(keyPressed);
    emit translateShuffledKeymapEntryRequested(charStr);
}
void AntiKeyAndMouseLoggerCli::handleShuffledKeymapEntryTranslated(const KeyMapEntry &translatedKeymapEntry)
{
    m_Password.append(translatedKeymapEntry);
}
