#ifndef STANDARDINPUTNOTIFIER_H
#define STANDARDINPUTNOTIFIER_H

#include <QObject>

#include <QTextStream>

#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
class QTimer;
#else
class QSocketNotifier;
#endif

class StandardInputNotifier : public QObject
{
    Q_OBJECT
public:
    explicit StandardInputNotifier(QObject *parent = 0);
    explicit StandardInputNotifier(int msecTimeoutForPollingStandardInput_WINDOWS_ONLY, QObject *parent = 0);
    ~StandardInputNotifier();

    static void setEchoStandardInput(bool echoStandardInput);
private:
    QTextStream *m_StandardInputTextStream;
    bool m_NotifyOnEmptyInput;
#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
    QTimer *m_StandardInputPollingTimerLoLWindows;
#else
    QSocketNotifier *m_StandardInputSocketNotifierWootEventDriven;
#endif

    void constructor(int msecTimeoutForPollingStandardInput_WINDOWS_ONLY);
signals:
    void standardInputReceivedLine(const QString &);
public slots:
    void setNotifyOnEmptyInput(bool notifyOnEmptyInput);

    //the user can just disconnect the signal (but actually this slot would still make rapid toggling easier, not that i need it :-P): void stopNotifyingOnStandardInput();
    void readStandardInAndEmitSignalIfNotEmpty();
};

#endif // STANDARDINPUTNOTIFIER_H
