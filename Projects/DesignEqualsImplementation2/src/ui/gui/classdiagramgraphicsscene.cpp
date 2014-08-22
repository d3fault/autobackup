#include "classdiagramgraphicsscene.h"

#include <QGraphicsSceneDragDropEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDataStream>

#include "designequalsimplementationguicommon.h"
#include "designequalsimplementationclassasqgraphicsitemforclassdiagramscene.h"
#include "../../designequalsimplementationproject.h"
#include "../../designequalsimplementationclass.h"

//TODOoptional: in the class editor, as well as having your traditional "add parameter" [to operation] drill downs like every other UML with separate easily distinguishable line edits, experimentally (AND USING libclang hopefully <3 <3) i should allow an operation to be quickly typed just as if it were C++. it is parsed before they exit the wizard, and they can confirm/modify the results. ex: typing "void someFuck(int sex, bool cuntNugget)" would be easily parsable by a fucking compiler library i would hope.... the hardest part will be trying to use libclang lol, but i bet if i joined their irc and asked "how to parse/separate a single function/method" I'd be pointed to exactly what I want :). There's really no limit to how much this app can interface with libclang, I'm spoiled by it
ClassDiagramGraphicsScene::ClassDiagramGraphicsScene(DesignEqualsImplementationProject *designEqualsImplementationProject)
    : IDesignEqualsImplementationGraphicsScene()
{
    privateConstructor(designEqualsImplementationProject);
}
ClassDiagramGraphicsScene::ClassDiagramGraphicsScene(DesignEqualsImplementationProject *designEqualsImplementationProject, const QRectF &sceneRect)
    : IDesignEqualsImplementationGraphicsScene(sceneRect)
{
    privateConstructor(designEqualsImplementationProject);
}
ClassDiagramGraphicsScene::ClassDiagramGraphicsScene(DesignEqualsImplementationProject *designEqualsImplementationProject, qreal x, qreal y, qreal width, qreal height)
    : IDesignEqualsImplementationGraphicsScene(x, y, width, height)
{
    privateConstructor(designEqualsImplementationProject);
}
ClassDiagramGraphicsScene::~ClassDiagramGraphicsScene()
{ }
void ClassDiagramGraphicsScene::handleAcceptedDropEvent(QGraphicsSceneDragDropEvent *event)
{
    QByteArray umlItemData = event->mimeData()->data(DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_CLASS_DIAGRAM_OBJECT);
    QDataStream dataStream(&umlItemData, QIODevice::ReadOnly);
    UmlItemsTypedef umlItemType;
    dataStream >> umlItemType;
    emit addUmlItemRequested(umlItemType, event->scenePos());
}
void ClassDiagramGraphicsScene::privateConstructor(DesignEqualsImplementationProject *designEqualsImplementationProject)
{
    m_CurrentProject = designEqualsImplementationProject;

    //requests
    connect(this, SIGNAL(addUmlItemRequested(UmlItemsTypedef,QPointF)), designEqualsImplementationProject, SLOT(handleAddUmlItemRequested(UmlItemsTypedef,QPointF)));

    //responses
    connect(designEqualsImplementationProject, SIGNAL(classAdded(DesignEqualsImplementationClass*)), this, SLOT(handleClassAdded(DesignEqualsImplementationClass*))); //TODOreq: deleting a part of a class in class diagram might completely invalidate a use case, so either that should trigger the entire deletion of said use case (after warning), or force them into going and fixing the use case, or similar
}
bool ClassDiagramGraphicsScene::wantDragDropEvent(QGraphicsSceneDragDropEvent *event)
{
    return (event->dropAction() == Qt::LinkAction && event->mimeData()->hasFormat(DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_CLASS_DIAGRAM_OBJECT));
}
void ClassDiagramGraphicsScene::handleClassAdded(DesignEqualsImplementationClass *classAdded)
{
    DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene *designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene = new DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene(classAdded, m_CurrentProject); //scene takes ownership at addItem
    designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene->setPos(classAdded->Position);
    //TODOreq: listen to move signals so the backend serializes later click-drag-rearranges from user

    connect(classAdded, SIGNAL(propertyAdded(DesignEqualsImplementationClassProperty*)), designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene, SLOT(handlePropertyAdded(DesignEqualsImplementationClassProperty*)));
    connect(classAdded, SIGNAL(slotAdded(DesignEqualsImplementationClassSlot*)), designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene, SLOT(handleSlotAdded(DesignEqualsImplementationClassSlot*)));
    connect(classAdded, SIGNAL(hasAPrivateMemberClassAdded(HasA_Private_Classes_Member*)), designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene, SLOT(handleHasAPrivateMemberClassAdded(HasA_Private_Classes_Member*)));

    //LISTS!?!?

    //QList<DesignEqualsImplementationClassPrivateMethod*> PrivateMethods;
    //QList<DesignEqualsImplementationClassSlot*> Slots;
    //QList<DesignEqualsImplementationClassSignal*> Signals;

    addItem(designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene); //TODOoptimization: when open existing project, perhaps addItem should be delayed until after the backend says "all project details emitted", so that tons of repaints aren't triggered. there's lots of solutions to this problem however
    QMetaObject::invokeMethod(classAdded, "emitAllClassDetails");
}
