#include "couchbasenotepadgui.h"

#include "objectonthreadgroup.h"
#include "couchbasenotepad.h"
#include "couchbasenotepadwidget.h"

CouchbaseNotepadGui::CouchbaseNotepadGui(QObject *parent)
    : QObject(parent)
    , m_Gui(0)
{
    ObjectOnThreadGroup *objectOnThreadGroup = new ObjectOnThreadGroup(this);
    objectOnThreadGroup->addObjectOnThread<CouchbaseNotepad>("handleCouchbaseNotepadReadyForConnections");
    objectOnThreadGroup->doneAddingObjectsOnThreads();
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

    //Requests
    connect(m_Gui, SIGNAL(getCouchbaseNotepadDocByKeyRequested(QString)), couchbaseNotepad, SLOT(getCouchbaseNotepadDocByKey(QString)));
    connect(m_Gui, SIGNAL(addCouchbaseNotepadDocByKeyRequested(QString,QString)), couchbaseNotepad, SLOT(addCouchbaseNotepadDocByKey(QString,QString)));

    //Responses
    connect(couchbaseNotepad, SIGNAL(getCouchbaseNotepadDocFinished(QString,QString)), m_Gui, SLOT(handleGetCouchbaseNotepadDocFinished(QString,QString)));
    connect(couchbaseNotepad, SIGNAL(addCouchbaseNotepadDocFinished(QString,QString)), m_Gui, SLOT(handleAddCouchbaseNotepadDocFinished(QString,QString)));

    m_Gui->show();
}
