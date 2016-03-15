#ifndef QTSYSTEMSIGNALHANDLER_H
#define QTSYSTEMSIGNALHANDLER_H

#include <QObject>

#include <QSet>

struct QtSystemSignal
{
    enum QtSystemSignalEnum
    {
          Unhandled = 0x0
        , Interrupt_aka_CtrlC = 0x1
        , TerminateGracefully_aka_KillPid = 0x2
        , WindowClosed_Win32Only = 0x4
        , Hangup_aka_ReloadConfigurationPlx = 0x8
        , Quit_aka_CtrlForwardSlash = 0x16 //or maybe it's backslash, fucking ambiguous name scheme, are we analyzing the top or the bottom of the slash?
        //TODOmb: USR1, USR2
    };
};
typedef QSet<QtSystemSignal::QtSystemSignalEnum> QtSystemSignals; //TODOoptional: QFlags instead
class QtSystemSignalHandler : public QObject
{
    Q_OBJECT
public:
    explicit QtSystemSignalHandler(QObject *parent = 0);
    ~QtSystemSignalHandler();
    static QtSystemSignals defaultSystemSignalsToHandle();
    static void setSystemSignalsToHandle(const QtSystemSignals &systemSignalsToHandle);

private:
    //buh, bug in c++ imo: friend class QScopedPointer<QtSystemSignalHandler>;
    //static QScopedPointer<QtSystemSignalHandler> s_Instance;

    static QtSystemSignalHandler *s_Instance;
    static QtSystemSignals s_SystemSignalsToHandle;

#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
    static BOOL WINAPI staticSignalHandler(DWORD sig);
    static QtSystemSignal::QtSystemSignalEnum osSignalToEnumSignal(DWORD sig);
#else
    static void staticSignalHandler(int sig);
    static QtSystemSignal::QtSystemSignalEnum osSignalToEnumSignal(int sig);
    static int enumSignalToOsSignal(QtSystemSignal::QtSystemSignalEnum sig);
#endif
signals:
    void systemSignalReceived(QtSystemSignal::QtSystemSignalEnum systemSignal);
private slots:
    void signalHandler(QtSystemSignal::QtSystemSignalEnum systemSignal);
};

#endif // QTSYSTEMSIGNALHANDLER_H
