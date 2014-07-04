#include "designequalsimplementationclassdiagramaswidgetfortab.h"

#include <QMutexLocker>

#include "designequalsimplementationclassasqgraphicsitemforclassdiagramscene.h"
#include "../../designequalsimplementation.h"
#include "../../designequalsimplementationproject.h"
#include "../../designequalsimplementationclass.h"

//TODOreq: at the time of writing, the only two uml items I can think of for class diagrams are "interfaces" and "classes", heh. Fucking UML spec is so bloated. Oh actually after looking at Dia just now, I think "notes" are pretty handy too (but I want to have them more "attached" to various objects instead of just eh floating)... but notes are hardly UML xD. OT: weird in Dia there are two "implements a specific interface" lines, and idfk the difference...
//TODOoptional: "search", which highlights uml items if they start with what the user types in
DesignEqualsImplementationClassDiagramAsWidgetForTab::DesignEqualsImplementationClassDiagramAsWidgetForTab(DesignEqualsImplementationProject *designEqualsImplementationProject, QWidget *parent)
    : IHaveAGraphicsViewAndScene(new ClassDiagramGraphicsScene(), parent)
{
    connect(static_cast<ClassDiagramGraphicsScene*>(m_GraphicsScene), SIGNAL(addUmlItemRequested(UmlItemsTypedef,QPointF)), designEqualsImplementationProject, SLOT(handleAddUmlItemRequested(UmlItemsTypedef,QPointF)));

    connect(designEqualsImplementationProject, SIGNAL(classAdded(DesignEqualsImplementationClass*)), this, SLOT(handleClassAdded(DesignEqualsImplementationClass*))); //TODOreq: deleting a part of a class in class diagram might completely invalidate a use case, so either that should trigger the entire deletion of said use case (after warning), or force them into going and fixing the use case, or similar
}
void DesignEqualsImplementationClassDiagramAsWidgetForTab::handleClassAdded(DesignEqualsImplementationClass *classAdded)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);

    DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene *designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene = new DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene(classAdded); //scene takes ownership at addItem
    designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene->setPos(classAdded->Position);

    connect(classAdded, SIGNAL(propertyAdded(DesignEqualsImplementationClassProperty*)), designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene, SLOT(handlePropertyAdded(DesignEqualsImplementationClassProperty*)));
    connect(classAdded, SIGNAL(hasAPrivateMemberClassAdded(HasA_PrivateMemberClasses_ListEntryType)), designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene, SLOT(handleHasAPrivateMemberClassAdded(HasA_PrivateMemberClasses_ListEntryType)));

    //LISTS!?!?

    //QList<DesignEqualsImplementationClassPrivateMethod*> PrivateMethods;
    //QList<DesignEqualsImplementationClassSlot*> Slots;
    //QList<DesignEqualsImplementationClassSignal*> Signals;

    m_GraphicsScene->addItem(designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene); //TODOreq: coordinates. TODOoptimization: when open existing project, perhaps addItem should be delayed until after the backend says "all project details emitted", so that tons of repaints aren't triggered. there's lots of solutions to this problem however
    QMetaObject::invokeMethod(classAdded, "emitAllClassDetails");
}
