#include "osiosuiapp.h"

#include <QCoreApplication>

#include "objectonthreadgroup.h"
#include "iosiosuifactory.h"
#include "iosiosui.h"
#include "../osios.h"

//"App" =  repetetive glue between UIs and core, can do same thread or put core or backend thread
OsiosUiApp::OsiosUiApp(IOsiosUiFactory *uiFactory, QObject *parent)
    : QObject(parent)
    , m_UiFactory(uiFactory)
    , m_OsiosThread(0)
{
    if(uiFactory->uiNeedsToBeOnDifferentThreadThanOsios())
    {
        //gui and web UIs put osios on separate thread, cli does not
        m_OsiosThread = new ObjectOnThreadGroup(this);
        m_OsiosThread->addObjectOnThread<Osios>("handleOsiosReadyForConnections"); //TODOoptimization: if not using this osios instance because another one is already open, we should clean up the thread/etc when that is determined
        m_OsiosThread->doneAddingObjectsOnThreads();
    }
    else
    {
        //cli UI, just do same thread
        handleOsiosReadyForConnections(new Osios(this));
    }
}
OsiosUiApp::~OsiosUiApp()
{
    delete m_UiFactory;
}
void OsiosUiApp::handleOsiosReadyForConnections(QObject *osiosAsQObject)
{
    Osios *osios = static_cast<Osios*>(osiosAsQObject);

    if(m_UiFactory->uiNeedsToBeOnDifferentThreadThanOsios())
    {
        connect(osios, SIGNAL(thisInstanceOfOsiosIsNotTheFirstOnThisMachine()), m_OsiosThread, SLOT(deleteLater()));
    }
    else
    {
        connect(osios, SIGNAL(thisInstanceOfOsiosIsNotTheFirstOnThisMachine()), osios, SLOT(deleteLater()));
    }

    IOsiosUi *ui = m_UiFactory->newIOsiosUi(this);
    connect(osios, SIGNAL(oisiosLocalServerReadyForIOsiosUiLocalSocketConnection(QString)), ui, SLOT(handleOisiosLocalServerReadyForIOsiosUiLocalSocketConnection(QString)));
    connect(osios, SIGNAL(quitRequestedBecauseNoMoreUiClientsConnected()), qApp, SLOT(quit()));
    connect(osios, SIGNAL(e(QString)), ui, SLOT(handleE(QString)));
    QMetaObject::invokeMethod(osios, "initializeAndStartOsios");

    //m_Gui->show(); //TODOreq: this show will probably come later after the backend determines state/session (tabs etc), but maybe not idfk
}
