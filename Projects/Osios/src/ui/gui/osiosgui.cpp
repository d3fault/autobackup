#include "osiosgui.h"

#include <QApplication>

#include "objectonthreadgroup.h"
#include "../../osios.h"
#include "osiosmainwindow.h"

OsiosGui::OsiosGui(QObject *parent)
    : IOsiosUi(parent)
{
    connect(qApp, SIGNAL(lastWindowClosed()), this, SLOT(handleThisUiIsDoneWithOsiosDaemon())); //TODOoptional: gui mode maybe is forced-system-tray until osios qlocalserver connection count drops to zero and it really quits (of course FORCE KILL is an option (warned against and never used by yours truly))
}
QObject *OsiosGui::presentUi()
{
    OsiosMainWindow *osiosMainWindow = new OsiosMainWindow();
    osiosMainWindow->show();
    return osiosMainWindow;
}
void OsiosGui::handleE(const QString &msg)
{
    //TODOreq
}
#if 0
void OsiosGui::handleLastWindowClosed()
{
    //do/start/finish persisting, eventually calling qapp quit when done

    //TODOreq: if a different client (cli,web) use relaying messages to us as a daemon.. as in, just if > 0 clients are connected, we don't exit at all until it becomes zero. HOWEVER, we can/do destroy the gui here/now (TODOoptional: don't ever destroy gui, just make it a forced system tray app)
    delete m_Gui;
}
#endif
