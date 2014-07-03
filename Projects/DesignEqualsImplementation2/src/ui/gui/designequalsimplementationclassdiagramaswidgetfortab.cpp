#include "designequalsimplementationclassdiagramaswidgetfortab.h"

#include <QVBoxLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMutexLocker>

#include "designequalsimplementationclassasqgraphicsitemforclassdiagramscene.h"
#include "../../designequalsimplementation.h"
#include "../../designequalsimplementationclass.h"

DesignEqualsImplementationClassDiagramAsWidgetForTab::DesignEqualsImplementationClassDiagramAsWidgetForTab(DesignEqualsImplementationProject *designEqualsImplementationProject, QWidget *parent)
    : QWidget(parent)
    , m_ClassDiagramScene(new QGraphicsScene(this))
{
    QVBoxLayout *myLayout = new QVBoxLayout();
    myLayout->addWidget(new QGraphicsView(m_ClassDiagramScene), 1); //TODOoptional: gl widget opt in at compile time, but if it ends up looking like shit then fuck it (perhaps a run-time switch that is only available if open gl was available at compile time)
    setLayout(myLayout);
}
void DesignEqualsImplementationClassDiagramAsWidgetForTab::handleClassAdded(DesignEqualsImplementationClass *classAdded)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    //TODOreq: draw the fucking class

    DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene *designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene = new DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene(classAdded); //TODOreq: no parent, so when deleted?

    connect(classAdded, SIGNAL(propertyAdded(DesignEqualsImplementationClassProperty*)), designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene, SLOT(handlePropertyAdded(DesignEqualsImplementationClassProperty*)));
    connect(classAdded, SIGNAL(hasAPrivateMemberClassAdded(HasA_PrivateMemberClasses_ListEntryType)), designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene, SLOT(handleHasAPrivateMemberClassAdded(HasA_PrivateMemberClasses_ListEntryType)));

    //LISTS!?!?

    //QList<DesignEqualsImplementationClassPrivateMethod*> PrivateMethods;
    //QList<DesignEqualsImplementationClassSlot*> Slots;
    //QList<DesignEqualsImplementationClassSignal*> Signals;

    m_ClassDiagramScene->addItem(designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene); //TODOreq: coordinates
    QMetaObject::invokeMethod(classAdded, "emitAllClassDetails");
}
