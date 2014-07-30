#include "simpleasynclibraryslotinvokeandsignalresponsegui.h"

#include <QCoreApplication>

#include "objectonthreadgroup.h"
#include "../../foo.h"
#include "simpleasynclibraryslotinvokeandsignalresponsewidget.h"

SimpleAsyncLibrarySlotInvokeAndSignalResponseGui::SimpleAsyncLibrarySlotInvokeAndSignalResponseGui(QObject *parent)
    : QObject(parent)
    , m_Gui(0)
{
    ObjectOnThreadGroup *threadGroup = new ObjectOnThreadGroup(this);
    threadGroup->addObjectOnThread<Foo>("handleFooReadyForConnections");
    threadGroup->doneAddingObjectsOnThreads();
}
SimpleAsyncLibrarySlotInvokeAndSignalResponseGui::~SimpleAsyncLibrarySlotInvokeAndSignalResponseGui()
{
    if(m_Gui)
        delete m_Gui;
}
void SimpleAsyncLibrarySlotInvokeAndSignalResponseGui::handleFooReadyForConnections(QObject *fooAsQObject)
{
    Foo *foo = static_cast<Foo*>(fooAsQObject);
    m_Gui = new SimpleAsyncLibrarySlotInvokeAndSignalResponseWidget();
    connect(m_Gui, SIGNAL(fooSlotRequested(QString)), foo, SLOT(fooSlot(QString)));
    //connect(foo, SIGNAL(diagnosticSignalX()),
    //connect(foo, SIGNAL(fooSignal(bool)), qApp, SLOT(quit()), Qt::QueuedConnection);
    connect(foo, SIGNAL(fooSignal(bool)), m_Gui, SLOT(handleFooSignal(bool)));
    m_Gui->show();
}
