#ifndef STANDARDINPUTNOTIFIER_H
#define STANDARDINPUTNOTIFIER_H

#include <QObject>

#include <QTextStream>

#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
#include <QTimer>
#else
#include <QSocketNotifier>
#include <QScopedPointer>
#include <termios.h>
#include <unistd.h>

struct UnsetAndDeleteTermios
{
    static inline void cleanup(termios *termiosToUnsetAndDelete)
    {
        //TODOreq: reset tty when ctrl+c/kill/etc
        (void)tcsetattr(STDIN_FILENO, TCSANOW, termiosToUnsetAndDelete);
        delete termiosToUnsetAndDelete;
    }
};
#endif

class StandardInputNotifier : public QObject
{
    Q_OBJECT
public:
    explicit StandardInputNotifier(QObject *parent = 0);
    explicit StandardInputNotifier(int msecTimeoutForPollingStandardInput_WINDOWS_ONLY, QObject *parent = 0);
    char getch();
    ~StandardInputNotifier();
private:
    QTextStream *m_StandardInputTextStream;
    bool m_NotifyOnEmptyInput;
#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
    QTimer *m_StandardInputPollingTimerLoLWindows;
#else
    QSocketNotifier *m_StandardInputSocketNotifierWootEventDriven;
    QScopedPointer<termios, UnsetAndDeleteTermios> m_OriginalTty;
    termios m_CurrentTty;

    void populateOriginalAndCurrentTtyIfNeeded();
#endif

    void constructor(int msecTimeoutForPollingStandardInput_WINDOWS_ONLY);
signals:
    void standardInputReceivedLine(const QString &);
public slots:
    void setNotifyOnEmptyInput(bool notifyOnEmptyInput);
    void setEchoStandardInput(bool echoStandardInput);
    void setDontWaitForEnterToBePressedAndEmitEveryCharacterIndividually(bool dontWaitForEnterToBePressedAndEmitEveryCharacterIndividually);

    //the user can just disconnect the signal (but actually this slot would still make rapid toggling easier, not that i need it :-P): void stopNotifyingOnStandardInput();
    void readStandardInAndEmitSignalIfNotEmpty();
};

#endif // STANDARDINPUTNOTIFIER_H
