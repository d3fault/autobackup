#include "antikeyandmouseloggercli.h"

#include <QCoreApplication>

#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
#include <conio.h> //might need more includes, maybe unistd?
#else
#include <termios.h>
#include <unistd.h>
#endif

#include "standardinputnotifier.h"

AntiKeyAndMouseLoggerCli::AntiKeyAndMouseLoggerCli(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
{
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
    return _getch();
#else
    char buf;
    if(read(STDIN_FILENO, &buf, 1) != 1)
    {
        //i want my destructor to run ;-P qFatal("didn't read exactly 1 character");
        m_StdOut << "Error: we didn't read exactly 1 character in myGetCh";
        //QMetaObject::invokeMethod(qApp, "quit");
        return static_cast<char>(static_cast<int>(10)); //return an 'Enter', so it's as if they chose to quit
    }
    return buf;
#endif
}
void AntiKeyAndMouseLoggerCli::presentShuffledKeymapPage(const KeyMap &shuffledKeymapPage)
{
    m_StdOut << endl << endl;
    int column = 1;
    //TODOreq: legend
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
