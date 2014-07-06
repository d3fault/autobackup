#include "usecasegraphicsscene.h"

#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QList>

#include <QMutexLocker>
#include "../../designequalsimplementation.h"

#include "designequalsimplementationguicommon.h"
#include "designequalsimplementationactorgraphicsitemforusecasescene.h"
#include "designequalsimplementationclasslifelinegraphicsitemforusecasescene.h"
#include "signalslotconnectionactivationarrowforgraphicsscene.h"
#include "designequalsimplementationclasslifelineunitofexecutiongraphicsitemforusecasescene.h"
#include "slotinvocationdialog.h"
#include "../../designequalsimplementationclass.h"
#include "../../designequalsimplementationactor.h"
#include "../../designequalsimplementationclasslifeline.h"

//TODOreq: if I put 2x Foos in the scene, and connected one to the other, wouldn't that be an infinite loop? Don't allow that if yes
//TODOreq: moving an item should make arrows move with it
UseCaseGraphicsScene::UseCaseGraphicsScene(DesignEqualsImplementationUseCase *useCase)
    : IDesignEqualsImplementationGraphicsScene()
{
    privateConstructor(useCase);
}
UseCaseGraphicsScene::UseCaseGraphicsScene(DesignEqualsImplementationUseCase *useCase, const QRectF &sceneRect)
    : IDesignEqualsImplementationGraphicsScene(sceneRect)
{
    privateConstructor(useCase);
}
UseCaseGraphicsScene::UseCaseGraphicsScene(DesignEqualsImplementationUseCase *useCase, qreal x, qreal y, qreal width, qreal height)
    : IDesignEqualsImplementationGraphicsScene(x, y, width, height)
{
    privateConstructor(useCase);
}
DesignEqualsImplementationMouseModeEnum UseCaseGraphicsScene::mouseMode() const
{
    return m_MouseMode;
}
UseCaseGraphicsScene::~UseCaseGraphicsScene()
{ }
void UseCaseGraphicsScene::handleAcceptedDropEvent(QGraphicsSceneDragDropEvent *event)
{
    QByteArray umlItemData = event->mimeData()->data(DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_USE_CASE_OBJECT);
    QDataStream dataStream(&umlItemData, QIODevice::ReadOnly);
    bool isActor;
    dataStream >> isActor;
    if(!isActor)
    {
        quintptr classBeingAddedAsQuintPtr;
        dataStream >> classBeingAddedAsQuintPtr;
        DesignEqualsImplementationClass *classBeingAdded = reinterpret_cast<DesignEqualsImplementationClass*>(classBeingAddedAsQuintPtr);
        emit addClassToUseCaseRequested(classBeingAdded, event->scenePos()); //TODOmb: since I'm uglily serializing a pointer, one way to ensure it's valid would be to invokeMethod on classBeingAdded. That would let/allow/make/harness/whatever Qt to do the safety checking for me :-P. Of course it'd ruin my design a tad if I went THROUGH class in order to add class to a use case...
    }
    else
    {
        emit addActorToUseCaseRequsted(event->scenePos()); //TODOoptional: bleh adding an actor is practically pointless [unless it's possible for use case is triggered by a signal (so nvm)], maybe i should add one right when the use case is created (nvmnvm). OK OK: how about if they drag a class onto a use case that has no actor or signal-entry-point, THEN i can auto-add an actor (or ask the user what they want (preemptive error detection))
    }
}
void UseCaseGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_MouseMode == DesignEqualsImplementationMouseDrawSignalSlotConnectionActivationArrowsMode)
    {
        if(event->button() != Qt::LeftButton)
        {
            //TODOreq: right clicking even in mouse mode should allow maybe something like "edit class" (if class (might be actor))
            return;
        }
        QTransform uselessTransform;
        QGraphicsItem *itemUnderMouse = itemAt(event->scenePos(), uselessTransform);
        //TODOreq: filter out other existing arrows
        if(itemUnderMouse) //for now i don't care what the item is (so long as it isn't existing arrow), i'll determine the source/dest on mouse release
        {
            //begin arrow drawing shit (yes, i am drunk). if you don't think i see the value of design= _BY ITSELF_, you're a fool. man i'm so.[
            //that advertising shit was boring
            //i can abstract multi threading concepts from software design, but i'll be damned if i can convince a girl to have a one night stand with me [without paying her money]
            //1-06-Gorillaz-Gorillaz-Man Research (Clapper).mp3 "ya ya ya ya ya ya" are teh best lyrics in gorillaz. the way he says it.
            m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn = new SignalSlotConnectionActivationArrowForGraphicsScene(QLineF(event->scenePos(), event->scenePos()));
            addItem(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn);
        }
    }
    else
    {
        QGraphicsScene::mousePressEvent(event);
    }
}
void UseCaseGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn)
    {
        QLineF newLine(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->line().p1(), event->scenePos());
        m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->setLine(newLine);
    }
    else
        QGraphicsScene::mouseMoveEvent(event);
}
void UseCaseGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //TODOreq: right now it's just Actor -> Class::slot
    if(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn)
    {
        QList<QGraphicsItem*> itemsUnderMouse = items(event->scenePos());
        if(!itemsUnderMouse.isEmpty() && itemsUnderMouse.first() == m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn)
            itemsUnderMouse.removeFirst();
        //TODOreq: filter out other existing arrows. TODOreq: if they put the destination arrow over the class NAME instead of a unit of execution (or even the thin life line thing), I should connect to a unit of execution that makes the most sense -- what that is right now I have no idea :-P
        //TODOreq: filter out source (just delete the arrow)
        if(!itemsUnderMouse.isEmpty())
        {
            //TODOreq: fix arrow so that it goes to the nearest edge, not the click+release points, of the source/dest items
            QLineF newLine(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->line().p1(), event->scenePos());
            m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->setLine(newLine); //TODOreq: should probably wait until the backend approves the connection as per reactor pattern... which probably means we delete/renew it... but idfk

            //I'm wondering if the GUI should do more handling (be smart) here or if we should just pass what we know to the backend and let him do all the decidering. I'm actually leaning towards the front-end, because for example we need to ask the user in a dialog if they want to do a simple slot invoke, or a named signal/slot activation (the signal could be created on the fly), or a simple signal-with-no-listeners-at-time-of-design emit. We need to ask the user in a dialog what the fuck they want to do (more specifically)! It's kind of worth noting (for me right now), but not always true, that it could be the first actor->slotInvocation (and in the future, signal-entry-point->slotInvocation), so in that case we need to limit what is presented to user (no named signals [created on the fly], for example). Since we have an object underneath the destination, we know it isn't a signal-with-no-listeners-at-time-of-design; those are handled below/differently
            //TODOoptional: a "drop down" widget thingo embedded right in the graphics scene would be better than a dialog imo, and while I'm on the subject allowing inline editting of classes without a dialog would be nice too!
            //TODOoptional: for now I'm going to KISS and use a modal dialog, but in the future I want to use modeless

            //TODOreq: determine that source is Actor before deciding to use SlotInvocationDialog
            QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
            SlotInvocationDialog slotInvocationDialog(static_cast<DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene*>(itemsUnderMouse.first())->unitOfExecution()); //TODOreq: segfault if drawing line to anything other than unit of execution lololol. TODOreq: i have 3 options, idk which makes the most sense: pass in unit of execution, pass in class lifeline, or pass i class. perhaps it doesn't matter... but for now to play it safe i'll pass in the unit of execution, since he has a reference to the other two :-P
            if(slotInvocationDialog.exec() == QDialog::Accepted)
            {
                //TODOreq: is more in line with reactor pattern to delete/redraw line once backend adds the use case event. in any case:
                emit addSlotInvocationUseCaseEventRequested(slotInvocationDialog.slotToInvoke(), slotInvocationDialog.slotInvocationContextVariables()); //TODOreq: makes sense that the unit of execution is emitted as well, but eh I'm kinda just tacking unit of execution on at this point and still don't see clearly how it fits in xD
            }
            else
            {
                delete m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn;
            }
        }
        else
        {
            //TODOreq: signals with no listeners at time of design
            delete m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn;
        }
        m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn = 0;
    }
    else
    {
        QGraphicsScene::mouseReleaseEvent(event);
    }
}
void UseCaseGraphicsScene::privateConstructor(DesignEqualsImplementationUseCase *useCase)
{
    m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn = 0;

    //requests
    connect(this, SIGNAL(addActorToUseCaseRequsted(QPointF)), useCase, SLOT(addActorToUseCase(QPointF)));
    connect(this, SIGNAL(addClassToUseCaseRequested(DesignEqualsImplementationClass*,QPointF)), useCase, SLOT(addClassToUseCase(DesignEqualsImplementationClass*,QPointF)));
    connect(this, SIGNAL(addSlotInvocationUseCaseEventRequested(DesignEqualsImplementationClassSlot*,SignalEmissionOrSlotInvocationContextVariables)), useCase, SLOT(addSlotInvocationEvent(DesignEqualsImplementationClassSlot*,SignalEmissionOrSlotInvocationContextVariables)));
    //TODOreq: scene, add use case event requested, use case, addUseCaseEvent

    //responses
    connect(useCase, SIGNAL(actorAdded(DesignEqualsImplementationActor*)), this, SLOT(handleActorAdded(DesignEqualsImplementationActor*)));
    connect(useCase, SIGNAL(classLifeLineAdded(DesignEqualsImplementationClassLifeLine*)), this, SLOT(handleClassLifeLineAdded(DesignEqualsImplementationClassLifeLine*)));
    connect(useCase, SIGNAL(eventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum,QObject*,SignalEmissionOrSlotInvocationContextVariables)), this, SLOT(handleEventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum,QObject*,SignalEmissionOrSlotInvocationContextVariables)));
}
bool UseCaseGraphicsScene::wantDragDropEvent(QGraphicsSceneDragDropEvent *event)
{
    return (event->dropAction() == Qt::LinkAction && event->mimeData()->hasFormat(DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_USE_CASE_OBJECT));
}
void UseCaseGraphicsScene::handleActorAdded(DesignEqualsImplementationActor *actor)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    //TODOreq
    //TODOreq: don't add more than 1 actor (it can be deleted and re-added however)

    DesignEqualsImplementationActorGraphicsItemForUseCaseScene *actorGraphicsItem = new DesignEqualsImplementationActorGraphicsItemForUseCaseScene(actor);
    actorGraphicsItem->setPos(actor->position());
    addItem(actorGraphicsItem);
}
void UseCaseGraphicsScene::handleClassLifeLineAdded(DesignEqualsImplementationClassLifeLine *newClassLifeLine)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);

    DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *designEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene = new DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(newClassLifeLine); //TODOreq: right-click -> change thread (main/gui thread (default), new thread X, existing thread Y). they should somehow visually indicate their thread. i was thinking a shared "filled-background" (color), but even if just the colors of the lifelines are the same that would be sufficient

    designEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene->setPos(newClassLifeLine->position()); //TODOreq: listen for moves

    addItem(designEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene);
}
void UseCaseGraphicsScene::handleEventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventTypeEnum, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionOrSlotInvocationContextVariables)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    //TODOreq: draw the fucking use case event blah
}
void UseCaseGraphicsScene::setMouseMode(DesignEqualsImplementationMouseModeEnum newMouseMode)
{
    m_MouseMode = newMouseMode;
}
