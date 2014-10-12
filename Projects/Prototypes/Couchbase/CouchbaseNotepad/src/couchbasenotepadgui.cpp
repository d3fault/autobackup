#include "couchbasenotepadgui.h"

#include <QCoreApplication>

#include "objectonthreadgroup.h"
#include "couchbasenotepad.h"
#include "couchbasenotepadwidget.h"

CouchbaseNotepadGui::CouchbaseNotepadGui(QObject *parent)
    : QObject(parent)
    , m_Gui(0)
{
    m_BackendThread = new ObjectOnThreadGroup(this);
    m_BackendThread->addObjectOnThread<CouchbaseNotepad>("handleCouchbaseNotepadReadyForConnections");
    m_BackendThread->doneAddingObjectsOnThreads();

    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
}
CouchbaseNotepadGui::~CouchbaseNotepadGui()
{
    if(m_Gui)
        delete m_Gui;
}
void CouchbaseNotepadGui::handleCouchbaseNotepadReadyForConnections(QObject *couchbaseNotepadAsQObject)
{
    CouchbaseNotepad *couchbaseNotepad = static_cast<CouchbaseNotepad*>(couchbaseNotepadAsQObject);

    //at this point i'm tempted to put the get/put stuff into an interface. RESISTING! KISS
    m_Gui = new CouchbaseNotepadWidget();

    connect(couchbaseNotepad, SIGNAL(e(QString)), m_Gui, SLOT(handleE(QString)));
    connect(couchbaseNotepad, SIGNAL(initializedAndStartedSuccessfully()), m_Gui, SLOT(handleInitializedAndStartedSuccessfully()));

    //Requests
    connect(m_Gui, SIGNAL(getCouchbaseNotepadDocByKeyRequested(QString)), couchbaseNotepad, SLOT(getCouchbaseNotepadDocByKey(QString)));
    connect(m_Gui, SIGNAL(addCouchbaseNotepadDocByKeyRequested(QString,QString)), couchbaseNotepad, SLOT(storeCouchbaseNotepadDocByKey(QString,QString)));

    //Responses
    connect(couchbaseNotepad, SIGNAL(getCouchbaseNotepadDocFinished(bool,bool,QString,QString)), m_Gui, SLOT(handleGetCouchbaseNotepadDocFinished(bool,bool,QString,QString)));
    connect(couchbaseNotepad, SIGNAL(addCouchbaseNotepadDocFinished(bool,bool,QString,QString)), m_Gui, SLOT(handleAddCouchbaseNotepadDocFinished(bool,bool,QString,QString)));

    connect(this, SIGNAL(quitBackendRequested()), couchbaseNotepad, SLOT(stopCouchbase()));

    m_Gui->show();

    QMetaObject::invokeMethod(couchbaseNotepad, "initializeAndStart", Qt::QueuedConnection);
}
void CouchbaseNotepadGui::handleAboutToQuit()
{
    emit quitBackendRequested();
    delete m_BackendThread; //quits and waits
}
