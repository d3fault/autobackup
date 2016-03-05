#include "qtsystemsignalhandler.h"

#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
#include <windows.h>
#else
#include <signal.h>
#endif

QScopedPointer<QtSystemSignalHandler> QtSystemSignalHandler::s_Instance;
QtSystemSignals QtSystemSignalHandler::s_SystemSignalsToHandle = defaultSystemSignalsToHandle();

//for win32 and unix this class works, but for 'other' systems this class should be ifdef'd out. and fuck it should be in qt itself imo
QtSystemSignalHandler::QtSystemSignalHandler(QObject *parent)
    : QObject(parent)
{
    s_SystemSignalsToHandle.remove(QtSystemSignal::Unhandled); //just in case they are retarded
#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
    SetConsoleCtrlHandler(QtSystemSignalHandler::staticSignalHandler, TRUE);
#else
    //we have to remove aliases otherwise we'll register the alias'ee twice. on win32 this doesn't matter because it has a 'catch all, then filter' approach... so the aliases will never be triggered from the os to begin with
    if(s_SystemSignalsToHandle.contains(QtSystemSignal::WindowClosed_Win32Only))
    {
        s_SystemSignalsToHandle.remove(QtSystemSignal::WindowClosed_Win32Only);
        s_SystemSignalsToHandle.insert(QtSystemSignal::TerminateGracefully_aka_KillPid);
    }

    Q_FOREACH(QtSystemSignal::QtSystemSignalEnum current, s_SystemSignalsToHandle)
    {
        struct sigaction sigAction;
        sigAction.sa_handler = QtSystemSignalHandler::staticSignalHandler;
        int systemSig = enumSignalToOsSignal(current);
        if(systemSig == -1)
            continue; //unhandled. maybe the "window closed win32 only" signal
        Q_ASSERT(sigaction(enumSignalToOsSignal(current), &sigAction, 0) == 0); //TODOmb: save the old action here and restore it in destructor
    }
#endif
}
void QtSystemSignalHandler::signalHandler(QtSystemSignal::QtSystemSignalEnum systemSignal)
{
    emit systemSignalReceived(systemSignal);
}
QtSystemSignalHandler::~QtSystemSignalHandler()
{
#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
    SetConsoleCtrlHandler(QtSystemSignalHandler::staticSignalHandler, FALSE);
#else
    Q_FOREACH(QtSystemSignal::QtSystemSignalEnum current, s_SystemSignalsToHandle)
    {
        signal(enumSignalToOsSignal(current), SIG_DFL); //using sigaction to register and then signal() to unregister is PROBBABLY fine, but honestly that's just a bold guess. worst case scenario: it explodes and all my files get deleted. there is a way to unregister it using sigaction again: the sigaction function lets you provide an optional param to store the old action in
    }
#endif
}
QtSystemSignals QtSystemSignalHandler::defaultSystemSignalsToHandle()
{
    QtSystemSignals ret;
    ret.insert(QtSystemSignal::Interrupt_aka_CtrlC);
    ret.insert(QtSystemSignal::TerminateGracefully_aka_KillPid);
    ret.insert(QtSystemSignal::WindowClosed_Win32Only);
    return ret;
}
void QtSystemSignalHandler::setSystemSignalsToHandle(const QtSystemSignals &systemSignalsToHandle)
{
    //this MUST be called before the first call to instance(), unless you want the default signals of course. but actually for win32 it can be called later (but for consistency/portability, it shouldn't)
    s_SystemSignalsToHandle = systemSignalsToHandle;
    s_SystemSignalsToHandle.remove(QtSystemSignal::Unhandled);
}
QtSystemSignalHandler *QtSystemSignalHandler::instance()
{
    if(s_Instance.isNull())
        s_Instance.reset(new QtSystemSignalHandler());
    return s_Instance.data();
}
#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
BOOL WINAPI QtSystemSignalHandler::staticSignalHandler(DWORD sig)
{
    if(!s_Instance.isNull())
    {
        QtSystemSignal::QtSystemSignalEnum sig = osSignalToEnumSignal(sig);
        if(s_SystemSignalsToHandle.contains(sig))
        {
            s_Instance->signalHandler(sig);
            return TRUE;
        }
    }
    return FALSE;
}
QtSystemSignal::QtSystemSignalEnum QtSystemSignalHandler::osSignalToEnumSignal(DWORD sig)
{
    switch(sig)
    {
    case CTRL_C_EVENT:
        return QtSystemSignal::Interrupt_aka_CtrlC;
    case CTRL_BREAK_EVENT:
        return QtSystemSignal::TerminateGracefully_aka_KillPid;
    case CTRL_CLOSE_EVENT:
        return QtSystemSignal::WindowClosed_Win32Only;
    default:
        return QtSystemSignal::Unhandled;
    }
}
#else
void QtSystemSignalHandler::staticSignalHandler(int sig)
{
    if(!s_Instance.isNull())
        s_Instance->signalHandler(osSignalToEnumSignal(sig));
}
QtSystemSignal::QtSystemSignalEnum QtSystemSignalHandler::osSignalToEnumSignal(int sig)
{
    switch(sig)
    {
    case SIGINT:
        return QtSystemSignal::Interrupt_aka_CtrlC;
    case SIGTERM:
        return QtSystemSignal::TerminateGracefully_aka_KillPid;
    case SIGHUP:
        return QtSystemSignal::Hangup_aka_ReloadConfigurationPlx;
    default:
        return QtSystemSignal::Unhandled;
    }
}
int QtSystemSignalHandler::enumSignalToOsSignal(QtSystemSignal::QtSystemSignalEnum sig)
{
    switch(sig)
    {
    case QtSystemSignal::Interrupt_aka_CtrlC:
        return SIGINT;
    case QtSystemSignal::TerminateGracefully_aka_KillPid:
    case QtSystemSignal::WindowClosed_Win32Only:
        return SIGTERM;
    case QtSystemSignal::Hangup_aka_ReloadConfigurationPlx:
        return SIGHUP;
    default:
        return -1;
    }
}
#endif
