#include "paginationofviewquerygui.h"

#include <QCoreApplication>

#include "objectonthreadgroup.h"
#include "paginationofviewquery.h"
#include "paginationofviewquerywidget.h"
#include "paginationofviewquerycommon.h"

PaginationOfViewQueryGui::PaginationOfViewQueryGui(QObject *parent)
    : QObject(parent)
    , m_Gui(0)
{
    qRegisterMetaType<ViewQueryPageContentsType>("ViewQueryPageContentsType");

    ObjectOnThreadGroup *backendThread = new ObjectOnThreadGroup(this);
    backendThread->addObjectOnThread<PaginationOfViewQuery>("handlePaginationOfViewQueryReadyForConnections");
    backendThread->doneAddingObjectsOnThreads();
}
PaginationOfViewQueryGui::~PaginationOfViewQueryGui()
{
    if(m_Gui)
        delete m_Gui;
}
void PaginationOfViewQueryGui::handlePaginationOfViewQueryInitialized(bool success)
{
    if(!success)
    {
        quit();
        return;
    }
    m_Gui->show();
}
void PaginationOfViewQueryGui::handlePaginationOfViewQueryReadyForConnections(QObject *paginationOfViewQueryAsQObject)
{
    PaginationOfViewQuery *business = static_cast<PaginationOfViewQuery*>(paginationOfViewQueryAsQObject);
    m_Gui = new PaginationOfViewQueryWidget();

    connect(business, SIGNAL(paginationOfViewQueryInitialized(bool)), this, SLOT(handlePaginationOfViewQueryInitialized(bool)));
    connect(m_Gui, SIGNAL(queryPageOfViewRequested(int)), business, SLOT(queryPageOfView(int)));
    connect(business, SIGNAL(finishedQueryingPageOfView(ViewQueryPageContentsType,bool)), m_Gui, SLOT(displayPageOfView(ViewQueryPageContentsType,bool)));
    connect(business, SIGNAL(quitRequested()), this, SLOT(quit()));

    QMetaObject::invokeMethod(business, "initializePaginationOfViewQuery");
}
void PaginationOfViewQueryGui::quit()
{
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
