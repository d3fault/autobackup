#include "usecasegraphicsscene.h"

#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QList>
#include <QScopedPointer>
#include <QGraphicsItem>

#include <QMutexLocker>
#include "../../designequalsimplementation.h"

#include "designequalsimplementationguicommon.h"
#include "designequalsimplementationactorgraphicsitemforusecasescene.h"
#include "designequalsimplementationclasslifelinegraphicsitemforusecasescene.h"
#include "signalslotconnectionactivationarrowforgraphicsscene.h"
#include "designequalsimplementationclasslifelineunitofexecutiongraphicsitemforusecasescene.h"
#include "signalslotmessagedialog.h"
#include "../../designequalsimplementationproject.h"
#include "../../designequalsimplementationclass.h"
#include "../../designequalsimplementationactor.h"
#include "../../designequalsimplementationclasslifeline.h"
#include "../../designequalsimplementationclasslifelineunitofexecution.h"

//TODOreq: if I put 2x Foos in the scene, and connected one to the other, wouldn't that be an infinite loop? Don't allow that if yes
//TODOreq: moving an item should make arrows move with it
// this struct calls "myCustomDeallocator" to delete the pointer
//OT: slotInvoke is used when a signal isn't warranted design-wise. The designer could be wrong or change their mind later and convert it to a signal/slot use case event, perhaps and probably after asking/prying from other developers. Still, pure slotInvokes help keep the API to a minimum (invoker not littered with corresponding [auto-generated] signals)
//TODOoptional: if dest hasA source (example: Foo hasA Bar), then we could perhaps have an "auto-generate handler (handleBarSignal) on dest" checkbox pre-checked" (there is no pushbutton, only this checkbox)... BECAUSE it is impossible (well, in this case at least) for Bar to invokeMethod whatever foo slot they choose. We KNOW they're going to want a slot, because that handler is now part of the use case / design
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

        HasA_Private_Classes_Members_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject = 0;
        //TODOreq: I either need to now iterate through all the classes to figure out who has me, OR determine that before being added to the use case uml items (in which case there could now be multiple Bars (Foo::m_Bar and Other::m_Bar2 (different instances (but not necessarily (oh god my brain))). I could still handle the multiple Bars scenario if I did the iterating-here-and-now method, I'd just ask the user if there was any ambiguity. It's easy to know that Foo hasA Bar once the connection is being drawn from Foo to Bar, BUT right now we're just adding Bar to the use case... so we can't know that it has anything to do with Foo just yet
        //I also need a way to specify that it's a top level object.... perhaps I can do that hackily by just saying it's the first non-actor class being added. (of course, that isn't true if it's a signal-slot-invoke-for-use-case-entry-point)

        //tempted to just do iteration method, but afraid I'll have to refactor later.... guh...
        //*checks what umbrello does for sequence diagrams*
        //lol they don't do anything even close. Bar is on the diagram nameless (just as Bar), and it's slot invocation is just the name of the slot (no args, and no m_Bar variable name like I'm trying to do)

        //I've decided that since they both sound so similar and yet the here/now one doesn't clutter up my uml items window, I'll do it here/now :-P
        QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
        QList<HasA_Private_Classes_Members_ListEntryType*> potentialExistencesInParents; //my father was a whore
        //was tempted momentarily to only iterate classes already in the use case, but nah
        Q_FOREACH(DesignEqualsImplementationClass *currentClass, m_UseCase->designEqualsImplementationProject()->classes())
        {
            Q_FOREACH(HasA_Private_Classes_Members_ListEntryType *currentClassCurrentHasA /*wat*/, currentClass->HasA_Private_Classes_Members)
            {
                if(currentClassCurrentHasA->m_DesignEqualsImplementationClass == classBeingAdded)
                {
                    potentialExistencesInParents.append(currentClassCurrentHasA);
                }
            }
        }
        if(!potentialExistencesInParents.isEmpty())
        {
            if(potentialExistencesInParents.size() > 1)
            {
                //TODOreq: modal dialog to choose which instance (TODOoptional: I could sort them by classes already added to use case before classes not added to use case :-P)
            }
            else
            {
                myInstanceInClassThatHasMe_OrZeroIfTopLevelObject = potentialExistencesInParents.first();
            }
        }

        emit addClassToUseCaseRequested(classBeingAdded, myInstanceInClassThatHasMe_OrZeroIfTopLevelObject, event->scenePos()); //TODOmb: since I'm uglily serializing a pointer, one way to ensure it's valid would be to invokeMethod on classBeingAdded. That would let/allow/make/harness/whatever Qt to do the safety checking for me :-P. Of course it'd ruin my design a tad if I went THROUGH class in order to add class to a use case...
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
        QGraphicsItem *itemUnderMouse = giveMeTopMostItemUnderPointThatIwantInArrowMouseMode_OrZeroIfNoneOfInterest(event->scenePos());
        if(itemUnderMouse) //for now i don't care what the item is (so long as it isn't existing arrow), i'll determine the source/dest on mouse release
        {
            //begin arrow drawing shit (yes, i am drunk). if you don't think i see the value of design= _BY ITSELF_, you're a fool. man i'm so.[
            //that advertising shit was boring
            //i can abstract multi threading concepts from software design, but i'll be damned if i can convince a girl to have a one night stand with me [without paying her money]
            //1-06-Gorillaz-Gorillaz-Man Research (Clapper).mp3 "ya ya ya ya ya ya" are teh best lyrics in gorillaz. the way he says it.
            m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn = new SignalSlotConnectionActivationArrowForGraphicsScene(itemUnderMouse, QLineF(event->scenePos(), event->scenePos()));
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
    if(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn) //holy shit a variable pun
    {
        //TODOreq: find "nearest point on source to current event scenepos", update line accordingly. TODOoptional: animation, but don't do any of those fancy curves that slow down or speed up at the beginning/end. only use the animation to smooth it out, nothing more. it should still be so fast that the user doesn't notice a different between having it turned off in terms of them waiting on it so they can continue designing (they are waiting for the animation before they release their mouse == bad scenario, go faster [or diable anims]!)

#if 0 //source position is locked in by now (wasn't working perfectly anyways), owned
         //TODOreq: i like it slippery with 1 pixel precision like it is now, but the "snapping" i refer to should just be like a "range that lights up". I'm ditching the orb idea because couldn't decide whether to put it on left or right side or both (but i want it to represent the entire row)
        QPointF mousePos = event->scenePos();
        QGraphicsItem *sourceGraphicsItem = m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->sourceGraphicsItem();
        const QPointF &sourcePosInScene = sourceGraphicsItem->scenePos();
        const QRectF &sourceBoundingRect = sourceGraphicsItem->sceneBoundingRect();
        qreal widthDiv2 = (sourceBoundingRect.width()/2);
        qreal heightDiv2 = (sourceBoundingRect.height()/2);
        qreal sourceLeftInScene = sourcePosInScene.x() - widthDiv2;
        qreal sourceRightInScene = sourcePosInScene.x() + widthDiv2;
        qreal sourceTopInScene = sourcePosInScene.y() - heightDiv2;
        qreal sourceBottomInScene = sourcePosInScene.y() + heightDiv2;
        QPointF nearestPoint(
                    //X:
                    qMin(qMax(mousePos.x(), sourceLeftInScene), sourceRightInScene),
                    //Y:
                    qMin(qMax(mousePos.y(), sourceTopInScene), sourceBottomInScene)
                    );
        QLineF newLine(nearestPoint, event->scenePos()); //TODOreq: snap the source before click and destination when mouse down pre-release (OLD: wanted to say both, but i'm not sure that's possible :-/) to points on the unit of execution, available positions visually seen as carved-out/empty meteor craters. used/non-available positions glowing red/orange and like an orb inside the meteor crater. RADIO BOXES use similar technique
#endif
        QLineF newLine(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->sourceGraphicsItem()->scenePos(), event->scenePos());
        m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->setLine(newLine);
    }
    else
        QGraphicsScene::mouseMoveEvent(event);
}
//TODOreq: I keep refactoring-while-writing mouseReleaseEvent because I keep changing my mind on what arrows connected to what (or none) objects should be allowed in the app. I pretty much will allow everything (class creation on the fly), but for now I'm going to just do the 3 modes that are vital do the design (signals, signal/slots, and invokeMethods). It's not that I don't want the others, but I'm having a bitch of a time focusing on what needs to be coded when I consider ALL the possible combinations together
void UseCaseGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //TODOreq: right now it's just slot invocation //these are the reqs we are looking for

    //QScopedPointer<SignalSlotConnectionActivationArrowForGraphicsScene*> autLineDeletionScopedDeleter(*m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn); //It has to be "taken" before end of this method which signifies a valid signal,signal-slot,or slotInvoke, nvm the custom deleter can't zero it out so...
    if(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn)
    {
        if(!keepArrowForThisMouseReleaseEvent(event))
        {
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
    m_UseCase = useCase;
    m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn = 0;

    //requests
    connect(this, SIGNAL(addActorToUseCaseRequsted(QPointF)), useCase, SLOT(addActorToUseCase(QPointF)));
    connect(this, SIGNAL(addClassToUseCaseRequested(DesignEqualsImplementationClass*,HasA_Private_Classes_Members_ListEntryType*,QPointF)), useCase, SLOT(addClassToUseCase(DesignEqualsImplementationClass*,HasA_Private_Classes_Members_ListEntryType*,QPointF)));
    connect(this, SIGNAL(addSlotInvocationUseCaseEventRequested(DesignEqualsImplementationClassSlot*,SignalEmissionOrSlotInvocationContextVariables)), useCase, SLOT(addSlotInvocationEvent(DesignEqualsImplementationClassSlot*,SignalEmissionOrSlotInvocationContextVariables)));
    connect(this, SIGNAL(addSignalSlotActivationUseCaseEventRequested(DesignEqualsImplementationClassSignal*,DesignEqualsImplementationClassSlot*,SignalEmissionOrSlotInvocationContextVariables)), useCase, SLOT(addSignalSlotActivationEvent(DesignEqualsImplementationClassSignal*,DesignEqualsImplementationClassSlot*,SignalEmissionOrSlotInvocationContextVariables)));
    connect(this, SIGNAL(addSignalEmissionUseCaseEventRequested(DesignEqualsImplementationClassSignal*,SignalEmissionOrSlotInvocationContextVariables)), useCase, SLOT(addSignalEmitEvent(DesignEqualsImplementationClassSignal*,SignalEmissionOrSlotInvocationContextVariables)));
    connect(this, SIGNAL(setExitSignalRequested(DesignEqualsImplementationClassSignal*,SignalEmissionOrSlotInvocationContextVariables)), useCase, SLOT(setExitSignal(DesignEqualsImplementationClassSignal*,SignalEmissionOrSlotInvocationContextVariables)));

    //responses
    connect(useCase, SIGNAL(actorAdded(DesignEqualsImplementationActor*)), this, SLOT(handleActorAdded(DesignEqualsImplementationActor*)));
    connect(useCase, SIGNAL(classLifeLineAdded(DesignEqualsImplementationClassLifeLine*)), this, SLOT(handleClassLifeLineAdded(DesignEqualsImplementationClassLifeLine*)));
    connect(useCase, SIGNAL(eventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum,QObject*,SignalEmissionOrSlotInvocationContextVariables)), this, SLOT(handleEventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum,QObject*,SignalEmissionOrSlotInvocationContextVariables)));
}
bool UseCaseGraphicsScene::keepArrowForThisMouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //first things first, get the topmost items [that we want] underneath the source and dest points
    QGraphicsItem *topMostItemIWantUnderSource = m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->sourceGraphicsItem(); //it was already set to zero if no items determined wanted
    if(!topMostItemIWantUnderSource)
        return false; //for now, we require a source
    QGraphicsItem *topMostItemIWantUnderDestination_CanBeZeroUnlessSourceIsActor = giveMeTopMostItemUnderPointThatIwantInArrowMouseMode_OrZeroIfNoneOfInterest(event->scenePos());
    if(!topMostItemIWantUnderDestination_CanBeZeroUnlessSourceIsActor && topMostItemIWantUnderSource->type() == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_Actor_GRAPHICS_TYPE_ID)
        return false;

    //Dialog mode is based entirely on whether or not there is a dest, in which case we always want to choose a slot on the dest (signal can be opted in to)
    DesignEqualsImplementationUseCase::UseCaseEventTypeEnum messageEditorDialogMode = (topMostItemIWantUnderDestination_CanBeZeroUnlessSourceIsActor == 0) ? DesignEqualsImplementationUseCase::UseCaseSignalEventType : DesignEqualsImplementationUseCase::UseCaseSignalSlotEventType;

    //TODOreq: if they put the destination arrow over the class NAME instead of a unit of execution (or even the thin life line thing), I should connect to a unit of execution that makes the most sense -- what that is right now? 1, since 1 is all I require right now. I almost put "1" at err IN the first unit of execution. Now you also see why 1 is all I require.

    //Redraw arrow at latest point
    //TODOreq: fix arrow so that it goes to the nearest edge, not the click+release points, of the source/dest items
    QLineF newLine(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->line().p1(), event->scenePos());
    m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->setLine(newLine); //TODOreq: should probably wait until the backend approves the connection as per reactor pattern... which probably means we delete/renew it... but idfk

    //I'm wondering if the GUI should do more handling (be smart) here or if we should just pass what we know to the backend and let him do all the decidering. I'm actually leaning towards the front-end, because for example we need to ask the user in a dialog if they want to do a simple slot invoke, or a named signal/slot activation (the signal could be created on the fly), or a simple signal-with-no-listeners-at-time-of-design emit. We need to ask the user in a dialog what the fuck they want to do (more specifically)! It's kind of worth noting (for me right now), but not always true, that it could be the first actor->slotInvocation (and in the future, signal-entry-point->slotInvocation), so in that case we need to limit what is presented to user (no named signals [created on the fly], for example). Since we have an object underneath the destination, we know it isn't a signal-with-no-listeners-at-time-of-design; those are handled below/differently
    //TODOoptional: a "drop down" widget thingo embedded right in the graphics scene would be better than a dialog imo, and while I'm on the subject allowing inline editting of classes without a dialog would be nice too!
    //TODOoptional: for now I'm going to KISS and use a modal dialog, but in the future I want to use modeless

    //TODOreq: determine that source is Actor before deciding to use SlotInvocationDialog (can still use SlotInvocationDialog if not actor, but that means the slot args must be filled in before the dialog can be accepted)
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    DesignEqualsImplementationClassLifeLineUnitOfExecution *targetUnitOfExecution_OrZeroIfNoDest = 0;
    bool destinationIsActor = false;
    if(topMostItemIWantUnderDestination_CanBeZeroUnlessSourceIsActor) //for now only units of executions can be dests (or nothing)
    {
        int topMostItemType = topMostItemIWantUnderDestination_CanBeZeroUnlessSourceIsActor->type();

        if(topMostItemType == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassLifeLineUnitOfExecution_GRAPHICS_TYPE_ID)
        {
            targetUnitOfExecution_OrZeroIfNoDest = qgraphicsitem_cast<DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene*>(topMostItemIWantUnderDestination_CanBeZeroUnlessSourceIsActor)->unitOfExecution();
        }

        if(topMostItemType == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_Actor_GRAPHICS_TYPE_ID)
        {
            destinationIsActor = true;
        }
    }

    bool sourceIsActor = (topMostItemIWantUnderSource->type() == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_Actor_GRAPHICS_TYPE_ID);
    if(sourceIsActor && destinationIsActor)
        return false;

    SignalSlotMessageDialog signalSlotMessageCreatorDialog(messageEditorDialogMode, targetUnitOfExecution_OrZeroIfNoDest, sourceIsActor, destinationIsActor, m_UseCase->SlotWithCurrentContext); //TODOreq: segfault if drawing line to anything other than unit of execution lololol. TODOreq: i have 3 options, idk which makes the most sense: pass in unit of execution, pass in class lifeline, or pass i class. perhaps it doesn't matter... but for now to play it safe i'll pass in the unit of execution, since he has a reference to the other two :-P
    if(signalSlotMessageCreatorDialog.exec() != QDialog::Accepted)
        return false;

    SignalEmissionOrSlotInvocationContextVariables signalEmissionOrSlotInvocationContextVariables = signalSlotMessageCreatorDialog.slotInvocationContextVariables();
    if(targetUnitOfExecution_OrZeroIfNoDest && targetUnitOfExecution_OrZeroIfNoDest->designEqualsImplementationClassLifeLine()->myInstanceInClassThatHasMe_OrZeroIfTopLevelObject())
    {
        signalEmissionOrSlotInvocationContextVariables.VariableNameOfObjectInCurrentContextWhoseSlotIsAboutToBeInvoked = targetUnitOfExecution_OrZeroIfNoDest->designEqualsImplementationClassLifeLine()->myInstanceInClassThatHasMe_OrZeroIfTopLevelObject()->VariableName; //If we don't have a target unit of execution, we are simple signal emit, and the backend knows that VariableName is of no use in that case
    }

    //TODOreq: we need to find out if the slotInvocation dialog was used as slotInvoke, signal/slot connection activation, or signal emit only
    DesignEqualsImplementationClassSignal *userChosenSignal_OrZeroIfNone = signalSlotMessageCreatorDialog.signalToEmit_OrZeroIfNone();
    DesignEqualsImplementationClassSlot *userChosenSlot_OrZeroIfNone = signalSlotMessageCreatorDialog.slotToInvoke_OrZeroIfNone();
    if(userChosenSlot_OrZeroIfNone && userChosenSignal_OrZeroIfNone)
    {
        //Signal/slot activation
        emit addSignalSlotActivationUseCaseEventRequested(userChosenSignal_OrZeroIfNone, userChosenSlot_OrZeroIfNone, signalEmissionOrSlotInvocationContextVariables);
        return true;
    }
    if(!userChosenSignal_OrZeroIfNone && userChosenSlot_OrZeroIfNone)
    {
        //Slot invocation
        //TODOreq: is more in line with reactor pattern to delete/redraw line once backend adds the use case event. in any case:
        emit addSlotInvocationUseCaseEventRequested(userChosenSlot_OrZeroIfNone, signalEmissionOrSlotInvocationContextVariables); //TODOreq: makes sense that the unit of execution is emitted as well, but eh I'm kinda just tacking unit of execution on at this point and still don't see clearly how it fits in xD
        return true;
    }
    if(destinationIsActor && userChosenSignal_OrZeroIfNone)
    {
        emit setExitSignalRequested(userChosenSignal_OrZeroIfNone, signalEmissionOrSlotInvocationContextVariables);
        return true;
    }
    if(!userChosenSlot_OrZeroIfNone && userChosenSignal_OrZeroIfNone)
    {
        //Signal with no listeners at time of design emit
        emit addSignalEmissionUseCaseEventRequested(userChosenSignal_OrZeroIfNone, signalEmissionOrSlotInvocationContextVariables);
        return true;
    }
    emit e("Error: Message editor dialog didn't give us anything we could work with");
    return false;
}
#if 0
    //if(m_UseCase->SlotWithCurrentContext)
#if 0
    if(topMostItemIWantInSource)
    {
        if
        {
            //source is actor
            //no context. for now i'm going to use this to tell me that it's the actor->firstSlotInvoke, but TODOreq: if they add two classes and no actor, that won't necessarily be true (we could have the GUI yell at them and force them to add an actor->slotInvoke first, BUT ideally the code would just be smarter and be able to handle it correctly)
            sourceIsActor = true;
        }
        else
        {
            //we have context
            sourceIsActor = false;
        }
#endif
    }
#if 0
    else
    {
        //TODOreq: uhh the create an actor or signal on the fly one (depending on if they populate signal)
    }
#endif
    //}
#if 0
    else
    {
        //TODOreq: signals with no listeners at time of design
        delete m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn;
    }
#endif
    m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn = 0;
//}
//returns 0 if no item, assumes m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn is not zero
#endif
QGraphicsItem* UseCaseGraphicsScene::giveMeTopMostItemUnderPointThatIwantInArrowMouseMode_OrZeroIfNoneOfInterest(QPointF pointToLookForItemsWeWant)
{
    QList<QGraphicsItem*> itemsUnderPoint = items(pointToLookForItemsWeWant);
    if(!itemsUnderPoint.isEmpty() && itemsUnderPoint.first() == m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn)
        itemsUnderPoint.removeFirst();
    QListIterator<QGraphicsItem*> it(itemsUnderPoint);
    while(it.hasNext())
    {
        QGraphicsItem *currentGraphicsItemUnderMouse = it.next();
        int itemType = currentGraphicsItemUnderMouse->type();
        if(itemType == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassLifeLine_GRAPHICS_TYPE_ID || itemType == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassLifeLineUnitOfExecution_GRAPHICS_TYPE_ID || itemType == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_Actor_GRAPHICS_TYPE_ID)
            return currentGraphicsItemUnderMouse; //TODOreq: when the user clicks or releases for the name up top of the class lifeline (the class name itself), find most natural use case unit class lifeline of execution for the arrow connection
        //TODOreq: etc
    }
    return 0;
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
