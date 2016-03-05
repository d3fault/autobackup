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
        //TODOmb: USR1, USR2
    };
};
typedef QSet<QtSystemSignal::QtSystemSignalEnum> QtSystemSignals;
class QtSystemSignalHandler : public QObject
{
    Q_OBJECT
public:
    static QtSystemSignals defaultSystemSignalsToHandle();
    static void setSystemSignalsToHandle(const QtSystemSignals &systemSignalsToHandle);
    static QtSystemSignalHandler *instance();

    ~QtSystemSignalHandler();
private:
    //buh, bug in c++ imo: friend class QScopedPointer<QtSystemSignalHandler>;
    explicit QtSystemSignalHandler(QObject *parent = 0);
    void signalHandler(QtSystemSignal::QtSystemSignalEnum systemSignal);

    static QScopedPointer<QtSystemSignalHandler> s_Instance;
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
};

#endif // QTSYSTEMSIGNALHANDLER_H
