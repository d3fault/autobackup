#include "qtsystemsignalhandler.h"

#include <QMetaType>

#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
#include <windows.h>
#else
#include <signal.h>
#endif

QtSystemSignalHandler* QtSystemSignalHandler::s_Instance = 0;
QtSystemSignals QtSystemSignalHandler::s_SystemSignalsToHandle = defaultSystemSignalsToHandle();

//for win32 and unix this class works, but for 'other' systems this class should be ifdef'd out. and fuck it should be in qt itself imo
QtSystemSignalHandler::QtSystemSignalHandler(QObject *parent)
    : QObject(parent)
{
    if(s_Instance != 0)
    {
        qFatal("Only one QtSystemSignalHandler instance allowed per app");
        return;
    }
    s_Instance = this;
    qRegisterMetaType<QtSystemSignal::QtSystemSignalEnum>("QtSystemSignal::QtSystemSignalEnum");

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
        if(sigaction(enumSignalToOsSignal(current), &sigAction, 0) != 0) //TODOmb: save the old action here and restore it in destructor
        {
            qFatal("sigaction() failed");
            return;
        }
    }
#endif
}
void QtSystemSignalHandler::signalHandler(QtSystemSignal::QtSystemSignalEnum systemSignal)
{
    emit systemSignalReceived(systemSignal);
}
QtSystemSignalHandler::~QtSystemSignalHandler()
{
    s_Instance = 0;
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
    ret.insert(QtSystemSignal::Quit_aka_CtrlForwardSlash);
    return ret;
}
void QtSystemSignalHandler::setSystemSignalsToHandle(const QtSystemSignals &systemSignalsToHandle)
{
    //this MUST be called before the first call to instance(), unless you want the default signals of course. but actually for win32 it can be called later (but for consistency/portability, it shouldn't)
    s_SystemSignalsToHandle = systemSignalsToHandle;
    s_SystemSignalsToHandle.remove(QtSystemSignal::Unhandled);
}
#if defined(Q_OS_WIN) || defined(Q_WS_WIN)
BOOL WINAPI QtSystemSignalHandler::staticSignalHandler(DWORD sig)
{
    if(s_Instance)
    {
        QtSystemSignal::QtSystemSignalEnum enumSig = osSignalToEnumSignal(sig);
        if(s_SystemSignalsToHandle.contains(enumSig))
        {
            QMetaObject::invokeMethod(s_Instance, "signalHandler", Qt::QueuedConnection, Q_ARG(QtSystemSignal::QtSystemSignalEnum, enumSig));
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
    if(s_Instance)
    {
        QtSystemSignal::QtSystemSignalEnum enumSig = osSignalToEnumSignal(sig);
        QMetaObject::invokeMethod(s_Instance, "signalHandler", Qt::QueuedConnection, Q_ARG(QtSystemSignal::QtSystemSignalEnum, enumSig)); //even though s_Instance lives on main thread and we are currently on the main thread, I've seen that signals are handled in a sort of... interrupt... kind of way. A slot currently executing is interrupted mid-execution in order to handle signals. Since I don't want to do that, I use a queued connection
    }
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
    case SIGQUIT:
        return QtSystemSignal::Quit_aka_CtrlForwardSlash;
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
    case QtSystemSignal::Quit_aka_CtrlForwardSlash:
        return SIGQUIT;
    default:
        return -1;
    }
}
#endif
