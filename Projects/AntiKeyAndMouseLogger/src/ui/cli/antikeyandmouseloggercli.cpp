#include "antikeyandmouseloggercli.h"

#include <QCoreApplication>

#ifdef Q_OS_WIN32
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
    setStdInEchoEnabled(false);

    AntiKeyAndMouseLogger *anti = new AntiKeyAndMouseLogger(this);
    connect(anti, SIGNAL(presentShuffledKeymapPageRequested(KeyMap)), this, SLOT(presentShuffledKeymapPage(KeyMap)));
    connect(this, SIGNAL(translateShuffledKeymapEntryRequested(QString)), anti, SLOT(translateShuffledKeymapEntry(QString)));
    connect(anti, SIGNAL(shuffledKeymapEntryTranslated(KeyMapEntry)), this, SLOT(handleShuffledKeymapEntryTranslated(KeyMapEntry)));

    StandardInputNotifier *sin = new StandardInputNotifier(this);
    sin->setNotifyOnEmptyInput(true);
    connect(sin, SIGNAL(standardInputReceivedLine(QString)), this, SLOT(handleStandardInputReceivedLine(QString)));

    anti->generateShuffledKeymapAndRequestPresentationOfFirstPage();
}
AntiKeyAndMouseLoggerCli::~AntiKeyAndMouseLoggerCli()
{
    m_StdOut << endl << tr("Password: ") << m_Password << endl; //TODOreq: ask them what to do with password. echo it, write it to a file, etc
    setStdInEchoEnabled(true); //mfw echo stays disabled even after the app finished xD. TODOreq: re-enable it when ctrl+c//kill/etc
}
void AntiKeyAndMouseLoggerCli::setStdInEchoEnabled(bool enabled)
{
    //TODOmb: stuff this code into StandardInputNotifier?
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
        tty.c_lflag &= ~ECHO;
    else
        tty.c_lflag |= ECHO;

    (void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
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
    m_StdOut << endl << tr("Enter Selection (leave blank to end program):") << endl;
    m_StdOut.flush();
}
void AntiKeyAndMouseLoggerCli::handleStandardInputReceivedLine(const QString &line)
{
    if(line.trimmed().isEmpty())
    {
        QMetaObject::invokeMethod(qApp, "quit");
        return;
    }
    QString firstCharOfLine = QString(line.at(0)); //discard everything but the first char. TODOreq make it so we read the char without them having to press enter
    emit translateShuffledKeymapEntryRequested(firstCharOfLine);
    //TODOmb: disconnect StandardInputNotifier until the backend responds, just like in Gui version? eh probably not necessary anyways (in either version) due to the way the event loop works, fuggit
}
void AntiKeyAndMouseLoggerCli::handleShuffledKeymapEntryTranslated(const KeyMapEntry &translatedKeymapEntry)
{
    m_Password.append(translatedKeymapEntry);
}
