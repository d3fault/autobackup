#include "usecasegraphicsscene.h"

#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QList>
#include <QScopedPointer>
#include <QGraphicsItem>
#include <QMap>
#include <QtMath>
#include <QMessageBox>

#include "designequalsimplementationguicommon.h"
#include "designequalsimplementationactorgraphicsitemforusecasescene.h"
#include "designequalsimplementationclasslifelinegraphicsitemforusecasescene.h"
#include "signalslotconnectionactivationarrowforgraphicsscene.h"
#include "designequalsimplementationclasslifelineunitofexecutiongraphicsitemforusecasescene.h"
#include "signalslotmessagedialog.h"
#include "snappingindicationvisualrepresentation.h"
#include "classinstancechooserdialog.h"
#include "../../designequalsimplementationproject.h"
#include "../../designequalsimplementationclass.h"
#include "../../designequalsimplementationactor.h"
#include "../../designequalsimplementationclasslifeline.h"
#include "../../designequalsimplementationclasslifelineunitofexecution.h"

#define UseCaseGraphicsScene_MOUSE_HOVER_SQUARE_SIDE_LENGTH 25

//TODOreq: if I put 2x Foos in the scene, and connected one to the other, wouldn't that be an infinite loop? Don't allow that if yes
//TODOreq: moving an item should make arrows move with it
// this struct calls "myCustomDeallocator" to delete the pointer
//OT: slotInvoke is used when a signal isn't warranted design-wise. The designer could be wrong or change their mind later and convert it to a signal/slot use case event, perhaps and probably after asking/prying from other developers. Still, pure slotInvokes help keep the API to a minimum (invoker not littered with corresponding [auto-generated] signals)
//TODOoptional: if dest hasA source (example: Foo hasA Bar), then we could perhaps have an "auto-generate handler (handleBarSignal) on dest" checkbox pre-checked" (there is no pushbutton, only this checkbox)... BECAUSE it is impossible (well, in this case at least) for Bar to invokeMethod whatever foo slot they choose. We KNOW they're going to want a slot, because that handler is now part of the use case / design
//TODOoptional: it makes sense that the "first slot" is not existing yet (but there's still the problems of how to add statements to it!).... what I'm getting at is that there should always be a "+1" slot/unit-of-execution ready for accepting arrow connections. creating it on mouse RELEASE is kinda ugly/hacky. when you have no stuf, all you see is that +1. After you've added stuff, at the bottom of every lifeline should be another pseudo-empty "+1" slot. My JIT create/destroy stuff would mostly stay the same (it must exist in order to add statements to it), BUT basically I'm just saying the GUI should lie about how many slots there are. When there are zero, it should show 1 (but still problem of connecting statements to "nameless" (not yet "invoked") slot, guh. I've flip flopped on this issue like a million times over the last few days, but I finally think the cleanest solution is to have a "ghost +1" to connect to. My existing "connect to slot and then JIT create one" is an ugly user-experience (kinda ;-P). Actually since typing that "kinda" I'm leaning back towards the other way ROFL. Because the user does not care about slots or units of execution!!! Showing a "ghost +1" might just confuse them ("where do I connect the line?")
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
        //TODOreq: I either need to now iterate through all the classes to figure out who has me, OR determine that before being added to the use case uml items (in which case there could now be multiple Bars (Foo::m_Bar and Other::m_Bar2 (different instances (but not necessarily (oh god my brain))). I could still handle the multiple Bars scenario if I did the iterating-here-and-now method, I'd just ask the user if there was any ambiguity. It's easy to know that Foo hasA Bar once the connection is being drawn from Foo to Bar, BUT right now we're just adding Bar to the use case... so we can't know that it has anything to do with Foo just yet
        //I also need a way to specify that it's a top level object.... perhaps I can do that hackily by just saying it's the first non-actor class being added. (of course, that isn't true if it's a signal-slot-invoke-for-use-case-entry-point)

        //tempted to just do iteration method, but afraid I'll have to refactor later.... guh...
        //*checks what umbrello does for sequence diagrams*
        //lol they don't do anything even close. Bar is on the diagram nameless (just as Bar), and it's slot invocation is just the name of the slot (no args, and no m_Bar variable name like I'm trying to do)

        //I've decided that since they both sound so similar and yet the here/now one doesn't clutter up my uml items window, I'll do it here/now :-P

#if 0 //TODOinstancing
        ClassInstanceChooserDialog classInstanceChooserDialog(classBeingAdded, m_UseCase);
        if(classInstanceChooserDialog.exec() != QDialog::Accepted)
            return;
        DesignEqualsImplementationClassInstance *myInstanceInClassThatHasMe = classInstanceChooserDialog.myInstanceInClassThatHasMe();
        if(classInstanceChooserDialog.newTopLevelInstanceChosen())
        {
            //TODOreq: hack: JIT create the top level object. not that hacky, but observer pattern should be used
            myInstanceInClassThatHasMe = m_UseCase->designEqualsImplementationProject()->createTopLevelClassInstance(classBeingAdded);
        }
#endif

#if 0

        QList<DesignEqualsImplementationClassInstance*> potentialExistencesInParents; //my father was a whore
        //was tempted momentarily to only iterate classes already in the use case, but nah
        Q_FOREACH(DesignEqualsImplementationClass *currentClass, m_UseCase->designEqualsImplementationProject()->classes())
        {
            Q_FOREACH(DesignEqualsImplementationClassInstance *currentClassCurrentHasA /*wat*/, currentClass->HasA_Private_Classes_Members)
            {
                if(currentClassCurrentHasA->m_MyClass == classBeingAdded)
                {
                    potentialExistencesInParents.append(currentClassCurrentHasA); //TODOreq: We're specifying a class as our parent, but we aren't specifying his instance!!! (mb we don't need to?)
                }
            }
        }
        if(!potentialExistencesInParents.isEmpty())
        {
            //TODOreq: modal dialog to choose which instance (TODOoptional: I could sort them by classes already added to use case before classes not added to use case :-P)
            InstanceChooserDialog instanceChooserDialog(classBeingAdded, potentialExistencesInParents);


            if(potentialExistencesInParents.size() > 1)
            {

            }
            else
            {
                myInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline = potentialExistencesInParents.first();
            }
        }
#endif

        emit addClassToUseCaseRequested(classBeingAdded, /*TODOinstancing: myInstanceInClassThatHasMe, */event->scenePos()); //TODOmb: since I'm uglily serializing a pointer, one way to ensure it's valid would be to invokeMethod on classBeingAdded. That would let/allow/make/harness/whatever Qt to do the safety checking for me :-P. Of course it'd ruin my design a tad if I went THROUGH class in order to add class to a use case...
    }
    else
    {
        emit addActorToUseCaseRequsted(event->scenePos()); //TODOoptional: bleh adding an actor is practically pointless [unless it's possible for use case is triggered by a signal (so nvm)], maybe i should add one right when the use case is created (nvmnvm). OK OK: how about if they drag a class onto a use case that has no actor or signal-entry-point, THEN i can auto-add an actor (or ask the user what they want (preemptive error detection))
    }
}
void UseCaseGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //maybe cleanup
    /*if(m_ItemThatDestinationSnappingForCurrentMousePosWillClick_OrZeroIfNone)
    {
        delete m_ItemThatDestinationSnappingForCurrentMousePosWillClick_OrZeroIfNone;
        m_ItemThatDestinationSnappingForCurrentMousePosWillClick_OrZeroIfNone = 0;
    }*/

    if(event->button() == Qt::LeftButton) //TODOreq: right clicking even in mouse mode should allow maybe something like "edit class" (if class (might be actor))
    {
        if(m_MouseMode == DesignEqualsImplementationMouseDrawSignalSlotConnectionActivationArrowsMode)
        {
            QPointF mouseEventScenePos = event->scenePos();

            if(m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone)
            {
                m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn = new SignalSlotConnectionActivationArrowForGraphicsScene(m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone->itemProxyingFor(), m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone->insertIndexForProxyItem(), QLineF(m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone->visualRepresentation()->scenePos(), event->scenePos()));
                addItem(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn);
                delete m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone;
                m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone = 0;
            }
            else
            {
                QGraphicsItem *itemUnderMouse = giveMeTopMostItemUnderPointThatIwantInArrowMouseMode_OrZeroIfNoneOfInterest(mouseEventScenePos);
                if(itemUnderMouse) //for now i don't care what the item is (so long as it isn't existing arrow), i'll determine the source/dest on mouse release
                {
                    //begin arrow drawing shit (yes, i am drunk). if you don't think i see the value of design= _BY ITSELF_, you're a fool. man i'm so.[
                    //that advertising shit was boring
                    //i can abstract multi threading concepts from software design, but i'll be damned if i can convince a girl to have a one night stand with me [without paying her money]
                    //1-06-Gorillaz-Gorillaz-Man Research (Clapper).mp3 "ya ya ya ya ya ya" are teh best lyrics in gorillaz. the way he says it.
                    int indexOnUnitOfExecutionThatStatementIsInsertedInto = 0;
                    if(itemUnderMouse->type() == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassSlot_GRAPHICS_TYPE_ID)
                    {
                        indexOnUnitOfExecutionThatStatementIsInsertedInto = static_cast<DesignEqualsImplementationSlotGraphicsItemForUseCaseScene*>(itemUnderMouse)->getInsertIndexForMouseScenePos(mouseEventScenePos);
                    }
                    m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn = new SignalSlotConnectionActivationArrowForGraphicsScene(itemUnderMouse, indexOnUnitOfExecutionThatStatementIsInsertedInto, QLineF(mouseEventScenePos, mouseEventScenePos));
                    addItem(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn);

                    /*if(m_ItemThatSnappingForCurrentMousePosWillClick_OrZeroIfNone) //There was a snap active, but we didn't use it
                    {
                        delete m_ItemThatSnappingForCurrentMousePosWillClick_OrZeroIfNone;
                        m_ItemThatSnappingForCurrentMousePosWillClick_OrZeroIfNone = 0;
                    }*/
                }
            }
            return;
        }
    }
    QGraphicsScene::mousePressEvent(event);
}
void UseCaseGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_MouseMode == DesignEqualsImplementationMouseDrawSignalSlotConnectionActivationArrowsMode && !m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn)
    {
        //Source snapping

        //TODOptimization: since "items" might be expensive, I could give it a 30ms/etc max poll frequency

        QPointF eventScenePos = event->scenePos();
        QRectF myMouseSnappingRect = mouseSnappingRect(eventScenePos);
        QList<QGraphicsItem*> itemsNearMouse = itemsIWantIntersectingRect(myMouseSnappingRect, m_ListOfItemTypesIWant_SnapSource);
        if(!itemsNearMouse.isEmpty())
        {
            //Find closest
            QGraphicsItem *itemWithEdgeNearestToPoint = findNearestPointOnItemBoundingRectFromPoint(itemsNearMouse, eventScenePos);

            DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *slotGraphicsItem = static_cast<DesignEqualsImplementationSlotGraphicsItemForUseCaseScene*>(itemWithEdgeNearestToPoint);
            if(m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone)
                delete m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone;
            m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone = static_cast<IRepresentSnapGraphicsItemAndProxyGraphicsItem*>(slotGraphicsItem->makeSnappingHelperForMousePoint(eventScenePos));
        }
        else if(m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone)
        {
            delete m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone;
            m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone = 0;
        }
    }

    if(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn) //holy shit a variable pun
    {
        QLineF newLine(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->line().p1(), event->scenePos());
        m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->setLine(newLine);

        //Destination snapping

        //TODOoptional: only allow the left OR right side to be allowed for destination snapping ("O"), depending on which direction the source is relative to us

        QPointF eventScenePos = event->scenePos();
        QRectF myMouseSnappingRect = mouseSnappingRect(event->scenePos());
        QList<QGraphicsItem*> itemsNearMouse = itemsIWantIntersectingRect(myMouseSnappingRect, m_ListOfItemTypesIWant_SnapDestination);
        if(!itemsNearMouse.isEmpty())
        {
            //Find closet
            QGraphicsItem *itemWithEdgeNearestToPoint = findNearestPointOnItemBoundingRectFromPoint(itemsNearMouse, eventScenePos);

            if(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->sourceGraphicsItem() == itemWithEdgeNearestToPoint)
                return;

            DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *slotGraphicsItem = static_cast<DesignEqualsImplementationSlotGraphicsItemForUseCaseScene*>(itemWithEdgeNearestToPoint);
            if(m_ArrowDestinationSnapper_OrZeroIfNone)
                delete m_ArrowDestinationSnapper_OrZeroIfNone;
            m_ArrowDestinationSnapper_OrZeroIfNone = slotGraphicsItem->makeSnappingHelperForSlotEntryPoint(eventScenePos);
        }
        else if(m_ArrowDestinationSnapper_OrZeroIfNone)
        {
            delete m_ArrowDestinationSnapper_OrZeroIfNone;
            m_ArrowDestinationSnapper_OrZeroIfNone = 0;
        }

        //TODOoptional: animation, but don't do any of those fancy curves that slow down or speed up at the beginning/end. only use the animation to smooth it out, nothing more. it should still be so fast that the user doesn't notice a different between having it turned off in terms of them waiting on it so they can continue designing (they are waiting for the animation before they release their mouse == bad scenario, go faster [or diable anims]!)

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
    }
    else if(m_ArrowDestinationSnapper_OrZeroIfNone)
    {
        delete m_ArrowDestinationSnapper_OrZeroIfNone;
        m_ArrowDestinationSnapper_OrZeroIfNone = 0;
    }
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
            delete m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn; //TODOreq: either always delete, or pass to backend to at least "add"/update for reactor pattern. Backend creates new units of execution (whenever target is slot) that the arrow must point to, but the unit of execution to point to doesn't exist until the backend tells us via signal
        }
        m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn = 0;
    }
    else
    {
        QGraphicsScene::mouseReleaseEvent(event);
    }

    //Dest snapping stops on mouse release
    /*if(m_ItemThatDestinationSnappingForCurrentMousePosWillClick_OrZeroIfNone)
    {
        delete m_ItemThatDestinationSnappingForCurrentMousePosWillClick_OrZeroIfNone;
        m_ItemThatDestinationSnappingForCurrentMousePosWillClick_OrZeroIfNone = 0;
    }*/
}
void UseCaseGraphicsScene::privateConstructor(DesignEqualsImplementationUseCase *useCase)
{
    m_UseCase = useCase;
    m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn = 0;
    m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone = 0;
    m_ArrowDestinationSnapper_OrZeroIfNone = 0;

    //TODOoptimization: make these lists static, no need for each use case to have a separate identical list
    m_ListOfItemTypesForArrowPressOrReleaseMode.append(DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassLifeLine_GRAPHICS_TYPE_ID);
    m_ListOfItemTypesForArrowPressOrReleaseMode.append(DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassSlot_GRAPHICS_TYPE_ID);
    m_ListOfItemTypesForArrowPressOrReleaseMode.append(DesignEqualsImplementationActorGraphicsItemForUseCaseScene_Actor_GRAPHICS_TYPE_ID);

    //m_ListOfItemTypesIWant_SnapSource.append(DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassLifeLine_GRAPHICS_TYPE_ID);
    m_ListOfItemTypesIWant_SnapSource.append(DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassSlot_GRAPHICS_TYPE_ID);

    //m_ListOfItemTypesIWant_SnapDestination.append(DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassLifeLine_GRAPHICS_TYPE_ID);
    m_ListOfItemTypesIWant_SnapDestination.append(DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassSlot_GRAPHICS_TYPE_ID);

    //TODOreq: snap to actor


    //requests
    connect(this, SIGNAL(addActorToUseCaseRequsted(QPointF)), useCase, SLOT(addActorToUseCase(QPointF)));
    connect(this, SIGNAL(addClassToUseCaseRequested(DesignEqualsImplementationClass*,/*TODOinstancing: DesignEqualsImplementationClassInstance*,*/QPointF)), useCase, SLOT(createClassLifelineInUseCase(DesignEqualsImplementationClass*,/*TODOinstancing: DesignEqualsImplementationClassInstance*,*/QPointF)));

    connect(this, SIGNAL(insertSlotInvocationUseCaseEventRequested(int,IDesignEqualsImplementationHaveOrderedListOfStatements*,DesignEqualsImplementationClassLifeLine*,DesignEqualsImplementationClassSlot*,SignalEmissionOrSlotInvocationContextVariables)), useCase, SLOT(insertSlotInvocationEvent(int,IDesignEqualsImplementationHaveOrderedListOfStatements*,DesignEqualsImplementationClassLifeLine*,DesignEqualsImplementationClassSlot*,SignalEmissionOrSlotInvocationContextVariables)));
    connect(this, SIGNAL(insertSignalSlotActivationUseCaseEventRequested(int,DesignEqualsImplementationClassSlot*,DesignEqualsImplementationClassSignal*,DesignEqualsImplementationClassSlot*,SignalEmissionOrSlotInvocationContextVariables,DesignEqualsImplementationClassLifeLine*,int)), useCase, SLOT(insertSignalSlotActivationEvent(int,DesignEqualsImplementationClassSlot*,DesignEqualsImplementationClassSignal*,DesignEqualsImplementationClassSlot*,SignalEmissionOrSlotInvocationContextVariables,DesignEqualsImplementationClassLifeLine*,int)));
    connect(this, SIGNAL(insertSignalEmissionUseCaseEventRequested(int,IDesignEqualsImplementationHaveOrderedListOfStatements*,DesignEqualsImplementationClassSignal*,SignalEmissionOrSlotInvocationContextVariables)), useCase, SLOT(insertSignalEmitEvent(int,IDesignEqualsImplementationHaveOrderedListOfStatements*,DesignEqualsImplementationClassSignal*,SignalEmissionOrSlotInvocationContextVariables)));

    connect(this, SIGNAL(setUseCaseSlotEntryPointRequested(DesignEqualsImplementationClassLifeLine*,DesignEqualsImplementationClassSlot*)), useCase, SLOT(setUseCaseSlotEntryPoint(DesignEqualsImplementationClassLifeLine*,DesignEqualsImplementationClassSlot*)));
    connect(this, SIGNAL(setExitSignalRequested(DesignEqualsImplementationClassSlot*,DesignEqualsImplementationClassSignal*,SignalEmissionOrSlotInvocationContextVariables)), useCase, SLOT(setExitSignal(DesignEqualsImplementationClassSlot*,DesignEqualsImplementationClassSignal*,SignalEmissionOrSlotInvocationContextVariables)));

    //responses
    connect(useCase, SIGNAL(actorAdded(DesignEqualsImplementationActor*)), this, SLOT(handleActorAdded(DesignEqualsImplementationActor*)));
    connect(useCase, SIGNAL(classLifeLineAdded(DesignEqualsImplementationClassLifeLine*)), this, SLOT(handleClassLifeLineAdded(DesignEqualsImplementationClassLifeLine*)));
    connect(useCase, SIGNAL(eventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum,QObject*,SignalEmissionOrSlotInvocationContextVariables)), this, SLOT(handleEventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum,QObject*,SignalEmissionOrSlotInvocationContextVariables)));

    if(m_UseCase->m_UseCaseActor_OrZeroIfNoneAddedYet)
    {
        handleActorAdded(m_UseCase->m_UseCaseActor_OrZeroIfNoneAddedYet);
    }
    Q_FOREACH(DesignEqualsImplementationClassLifeLine *currentClassLifeline, useCase->m_ClassLifeLines)
    {
        handleClassLifeLineAdded(currentClassLifeline);
    }
}
//TODOreq: if new slot jit created and shown behind message editor, the arrow's p2 should be updated to reflect that. HOWEVER I'm considering using ghosting techniques to already show what will happen should the user release the mouse button, so those considerations need to be considered
//TODOreq: certain actions occuring right around the message editor dialog, such as jit slot creation, should be undone if the dialog is cancelled xD
bool UseCaseGraphicsScene::keepArrowForThisMouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //first things first, get the topmost items [that we want] underneath the source and dest points
    QGraphicsItem *sourceItem = m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->sourceGraphicsItem(); //it was already set to zero if no items determined wanted
    if(!sourceItem)
        return false; //for now, we require a source
    QGraphicsItem *destinationItem_CanBeZeroUnlessSourceIsActor = 0;
    if(m_ArrowDestinationSnapper_OrZeroIfNone)
    {
        destinationItem_CanBeZeroUnlessSourceIsActor = m_ArrowDestinationSnapper_OrZeroIfNone->itemProxyingFor();
        QLineF existingLine = m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->line();
        existingLine.setP2(m_ArrowDestinationSnapper_OrZeroIfNone->visualRepresentation()->scenePos());
        m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->setLine(existingLine);
        delete m_ArrowDestinationSnapper_OrZeroIfNone;
        m_ArrowDestinationSnapper_OrZeroIfNone = 0;
    }
    else
    {
        destinationItem_CanBeZeroUnlessSourceIsActor = giveMeTopMostItemUnderPointThatIwantInArrowMouseMode_OrZeroIfNoneOfInterest(event->scenePos());
    }
    int sourceItemType = sourceItem->type();
    bool sourceIsActor = (sourceItemType == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_Actor_GRAPHICS_TYPE_ID);
    if(!destinationItem_CanBeZeroUnlessSourceIsActor && sourceIsActor)
        return false;    

    //Dialog mode is based entirely on whether or not there is a dest, in which case we always want to choose a slot on the dest (signal can be opted in to)
    DesignEqualsImplementationUseCase::UseCaseEventTypeEnum messageEditorDialogMode = (destinationItem_CanBeZeroUnlessSourceIsActor == 0) ? DesignEqualsImplementationUseCase::UseCaseSignalEventType : DesignEqualsImplementationUseCase::UseCaseSignalSlotEventType;

    //TODOreq: if they put the destination arrow over the class NAME instead of a unit of execution (or even the thin life line thing), I should connect to a unit of execution that makes the most sense -- what that is right now? 1, since 1 is all I require right now. I almost put "1" at err IN the first unit of execution. Now you also see why 1 is all I require.

    //Redraw arrow at latest point
    //TODOreq: fix arrow so that it goes to the nearest edge, not the click+release points, of the source/dest items
    //QLineF newLine(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->line().p1(), event->scenePos());
    //m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->setLine(newLine); //TODOreq: wait until backend approves the connection as per reactor pattern... and then set the line based on where the [maybe generated on the fly] slot graphics item actually is

    //I'm wondering if the GUI should do more handling (be smart) here or if we should just pass what we know to the backend and let him do all the decidering. I'm actually leaning towards the front-end, because for example we need to ask the user in a dialog if they want to do a simple slot invoke, or a named signal/slot activation (the signal could be created on the fly), or a simple signal-with-no-listeners-at-time-of-design emit. We need to ask the user in a dialog what the fuck they want to do (more specifically)! It's kind of worth noting (for me right now), but not always true, that it could be the first actor->slotInvocation (and in the future, signal-entry-point->slotInvocation), so in that case we need to limit what is presented to user (no named signals [created on the fly], for example). Since we have an object underneath the destination, we know it isn't a signal-with-no-listeners-at-time-of-design; those are handled below/differently
    //TODOoptional: a "drop down" widget thingo embedded right in the graphics scene would be better than a dialog imo, and while I'm on the subject allowing inline editting of classes without a dialog would be nice too!
    //TODOoptional: for now I'm going to KISS and use a modal dialog, but in the future I want to use modeless

    //TODOreq: determine that source is Actor before deciding to use SlotInvocationDialog (can still use SlotInvocationDialog if not actor, but that means the slot args must be filled in before the dialog can be accepted)
    DesignEqualsImplementationClassSlot *destinationSlotIsProbablyNameless_OrZeroIfNoDest = 0;
    DesignEqualsImplementationClassLifeLine *destinationClassLifeLine_OrZeroIfNoDest = 0;
    DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *destinationClassLifeLineGraphicsItem_OrZeroIfNoDest = 0;
    DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *destinationSlotGraphicsItem_OrZeroIfNoDest = 0;
    bool destinationIsActor = false;
    if(destinationItem_CanBeZeroUnlessSourceIsActor) //for now only units of executions can be dests (or nothing)
    {
        int destinationItemType = destinationItem_CanBeZeroUnlessSourceIsActor->type();

        if(destinationItemType == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_Actor_GRAPHICS_TYPE_ID)
        {
            destinationIsActor = true;
        }
        if(destinationItemType == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassLifeLine_GRAPHICS_TYPE_ID)
        {
            destinationClassLifeLineGraphicsItem_OrZeroIfNoDest = static_cast<DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene*>(destinationItem_CanBeZeroUnlessSourceIsActor);
            destinationClassLifeLine_OrZeroIfNoDest = destinationClassLifeLineGraphicsItem_OrZeroIfNoDest->classLifeLine();
        }
        if(destinationItemType == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassSlot_GRAPHICS_TYPE_ID)
        {
            destinationSlotGraphicsItem_OrZeroIfNoDest = static_cast<DesignEqualsImplementationSlotGraphicsItemForUseCaseScene*>(destinationItem_CanBeZeroUnlessSourceIsActor);
            destinationSlotIsProbablyNameless_OrZeroIfNoDest = destinationSlotGraphicsItem_OrZeroIfNoDest->underlyingSlot();
            destinationClassLifeLineGraphicsItem_OrZeroIfNoDest = destinationSlotGraphicsItem_OrZeroIfNoDest->parentClassLifelineGraphicsItem();
            destinationClassLifeLine_OrZeroIfNoDest = destinationClassLifeLineGraphicsItem_OrZeroIfNoDest->classLifeLine();
        }

        //if(destinationSlotIsProbablyNameless_OrZeroIfNoDest && sourceSlotForStatementInsertion_OrZeroIfSourceIsActor && destinationSlotIsProbablyNameless_OrZeroIfNoDest == )

        if(!destinationIsActor)
        {
            if(destinationSlotGraphicsItem_OrZeroIfNoDest) //TODOreq: skipping slot entry point connecting (ie: Foo::unnamedFirstSlot -> Bar::fooSlot -> Bar::barSignal -> Foo::handleBarSignal) should not now name that first Foo::unnamedFirstSlot, because if it did then it would be an infinite loop. We need to see that since unnamedFirstSlot referenced us "later on", that our NAMING him would be an infinite loop. To KISS initially, I should just always create a new slot whenever the dest's statement list isn't empty (adding that to this if now)
            {
                bool jitCreate = true;
                if(destinationSlotIsProbablyNameless_OrZeroIfNoDest->Name == UseCaseGraphicsScene_TEMP_SLOT_MAGICAL_NAME_STRING)
                {
                    jitCreate = false;
                }
#if 0
                if(!destinationSlotIsProbablyNameless_OrZeroIfNoDest->orderedListOfStatements().isEmpty())
                {
                    //for now always jit create when the statement list isn't empty
                }
#endif

                if(jitCreate)
                {
                    //jit create slot on class lifeline in destination (unless it's actor)
                    DesignEqualsImplementationClassSlot *newSlot = destinationSlotIsProbablyNameless_OrZeroIfNoDest->ParentClass->createwNewSlot(UseCaseGraphicsScene_TEMP_SLOT_MAGICAL_NAME_STRING);
                    if(destinationClassLifeLine_OrZeroIfNoDest)
                    {
                        destinationClassLifeLine_OrZeroIfNoDest->insertSlotToClassLifeLine(destinationClassLifeLine_OrZeroIfNoDest->mySlotsAppearingInClassLifeLine().size(), newSlot);
                    }
                    destinationSlotIsProbablyNameless_OrZeroIfNoDest = newSlot;
                    destinationSlotGraphicsItem_OrZeroIfNoDest = 0;
                    //destinationSlotGraphicsItem_OrZeroIfNoDest is now invalid, but it isn't referenced anymore so whatever..
                    //TODOreq: reposition arrow p2 to the NEW destination slot graphics item, except we have to wait for it to be added guh
                }
            }
        }
#if 0
        DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *lifelineGraphicsItem;
        if(destinationItemType == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassSlot_GRAPHICS_TYPE_ID)
        {
                lifelineGraphicsItem = qgraphicsitem_cast<DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene*>(topMostItemIWantUnderDestination_CanBeZeroUnlessSourceIsActor)->parentClassLifeline();
            }
            else if(destinationItemType == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassLifeLine_GRAPHICS_TYPE_ID)
            {
                lifelineGraphicsItem = qgraphicsitem_cast<DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene*>(topMostItemIWantUnderDestination_CanBeZeroUnlessSourceIsActor);
            }
            else
                return false;

            //TODOreq: nothing should happen until AFTER the dialog is accepted
            DesignEqualsImplementationClassLifeLine *classLifeline = lifelineGraphicsItem->classLifeLine();
            classLifeline->unitsOfExecution().append(new DesignEqualsImplementationClassLifeLineUnitOfExecution(classLifeline, classLifeline)); //TODOreq: not sure if this would COW the units of execution list or not -_-
        }
#endif
    }

    if(sourceIsActor && destinationIsActor)
        return false;

    //DesignEqualsImplementationClass *sourceClass = 0;
    DesignEqualsImplementationClassSlot *sourceSlotForStatementInsertion_OrZeroIfSourceIsActor = 0;
    DesignEqualsImplementationClassLifeLine *sourceClassLifeLine_OrZeroIfSourceIsActor = 0;
    if(!sourceIsActor)
    {
        if(sourceItemType == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassSlot_GRAPHICS_TYPE_ID)
        {
            DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *sourceSlotGraphicsItem = static_cast<DesignEqualsImplementationSlotGraphicsItemForUseCaseScene*>(sourceItem);
            sourceSlotForStatementInsertion_OrZeroIfSourceIsActor = sourceSlotGraphicsItem->underlyingSlot();
            sourceClassLifeLine_OrZeroIfSourceIsActor = sourceSlotGraphicsItem->parentClassLifelineGraphicsItem()->classLifeLine();
        }
        else if(sourceItemType == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassLifeLine_GRAPHICS_TYPE_ID)
        {
            //If they click-drag an arrow from the "class name" graphics item, the last unit of execution is selected for the statement append. This may prove to be undesireable behavior, but honestly I like the idea of just drawing lines from the class names back and forth knowing each one is APPENDED to unit of execution etc, without having to scroll down etc to see it all
            sourceClassLifeLine_OrZeroIfSourceIsActor = static_cast<DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene*>(sourceItem)->classLifeLine();
            if(!sourceClassLifeLine_OrZeroIfSourceIsActor->mySlotsAppearingInClassLifeLine().isEmpty())
            {
                sourceSlotForStatementInsertion_OrZeroIfSourceIsActor = sourceClassLifeLine_OrZeroIfSourceIsActor->mySlotsAppearingInClassLifeLine().last(); //TODOreq: last() can't be right (but i have no context atm (wrote the code a while ago) so idfk). The context is: arrow release on class name box (not slot), so "last()" is decent solution, but i can do better i know "last UNNAMED" is better, but whatever
                //sourceUnitOfExecution_OrZeroIfSourceIsActorOrNotWantedType = classLifeLine->targetUnitOfExecutionIfUnitofExecutionIsUnnamed_FirstAfterTargetIfNamedEvenIfYouHaveToCreateIt();
            }
        }
    }

    if(sourceSlotForStatementInsertion_OrZeroIfSourceIsActor == destinationSlotIsProbablyNameless_OrZeroIfNoDest)
        return false; //Drag arrow onto self = unsupported for now. TODOoptional: call private methods this way (with arch arrow)?

    //TODOreq: by/at-around now, we know which index in the source the statement should be inserted at. We don't have to give it to the dialog, but we do need to emit it to the backend after dialog is accepted
    int indexToInsertStatementAt_IntoSource = m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->statementInsertIndex();

    SignalSlotMessageDialog signalSlotMessageCreatorDialog(messageEditorDialogMode, destinationSlotIsProbablyNameless_OrZeroIfNoDest, sourceIsActor, destinationIsActor, sourceClassLifeLine_OrZeroIfSourceIsActor, destinationClassLifeLine_OrZeroIfNoDest, sourceSlotForStatementInsertion_OrZeroIfSourceIsActor); //TODOreq: segfault if drawing line to anything other than unit of execution lololol. TODOreq: i have 3 options, idk which makes the most sense: pass in unit of execution, pass in class lifeline, or pass i class. perhaps it doesn't matter... but for now to play it safe i'll pass in the unit of execution, since he has a reference to the other two :-P
    if(signalSlotMessageCreatorDialog.exec() != QDialog::Accepted)
        return false;
    SignalEmissionOrSlotInvocationContextVariables signalEmissionOrSlotInvocationContextVariables = signalSlotMessageCreatorDialog.slotInvocationContextVariables();

    //Retrieved specified variable name when slot invoke
#if 0 //TODOinstancing
    if(destinationSlotIsProbablyNameless_OrZeroIfNoDest && destinationClassLifeLine_OrZeroIfNoDest && destinationClassLifeLine_OrZeroIfNoDest->myInstanceInClassThatHasMe())
    {
        signalEmissionOrSlotInvocationContextVariables.VariableNameOfObjectInCurrentContextWhoseSlotIsAboutToBeInvoked = destinationClassLifeLine_OrZeroIfNoDest->myInstanceInClassThatHasMe()->VariableName; //If we don't have a target unit of execution, we are simple signal emit, and the backend knows that VariableName is of no use in that case
    }
#endif

    //TODOreq: we need to find out if the slotInvocation dialog was used as slotInvoke, signal/slot connection activation, or signal emit only
    DesignEqualsImplementationClassSignal *userChosenSourceSignal_OrZeroIfNone = signalSlotMessageCreatorDialog.signalToEmit_OrZeroIfNone();
    DesignEqualsImplementationClassSlot *userChosenDestinationSlot_OrZeroIfNone = signalSlotMessageCreatorDialog.slotToInvoke_OrZeroIfNone();

#if 0
    if(userChosenDestinationSlot_OrZeroIfNone)
    {
        userChosenDestinationSlot_OrZeroIfNone->setParentClassLifeLineInUseCaseView_OrZeroInClassDiagramView_OrZeroWhenFirstTimeSlotIsUsedInAnyUseCaseInTheProject(destinationSlotIsProbablyNameless_OrZeroIfNoDest->parentClassLifeLineInUseCaseView_OrZeroInClassDiagramView_OrZeroWhenFirstTimeSlotIsUsedInAnyUseCaseInTheProject()); //TODOreq: should probably be "addClassParentLifeLineRef"
    }
#endif

    if(destinationSlotIsProbablyNameless_OrZeroIfNoDest)
    {

        //HACK to delete target slot if unnamed and empty
        if(userChosenDestinationSlot_OrZeroIfNone != destinationSlotIsProbablyNameless_OrZeroIfNoDest && destinationSlotIsProbablyNameless_OrZeroIfNoDest->Name == UseCaseGraphicsScene_TEMP_SLOT_MAGICAL_NAME_STRING && destinationClassLifeLine_OrZeroIfNoDest)
        {
            //TODOoptimization: could sneak the slot in without deleting the graphics item and recreating it, but as of now it is done transparently via reactor pattern through two below calls. Btw I'm putting business logic in the gui but fuck it this is deserving and idgaf anymore (TODOimplicitsharing: request here that the next two statements are performed in the backend... but wtf it needs to be finished before i delete it 3 lines down so idfk)

            //TODOreq: bring all existing statements over to the userChosenDestinationSlot_OrZeroIfNone, modal dialog asking for merge strategy if userChosenDestinationSlot_OrZeroIfNone already has existing statements and targetSlotUnderGraphicsItemIsProbablyNameless_OrZeroIfNoDest does too
            if(!userChosenDestinationSlot_OrZeroIfNone->orderedListOfStatements().isEmpty() && !destinationSlotIsProbablyNameless_OrZeroIfNoDest->orderedListOfStatements().isEmpty())
            {
                QMessageBox::information(0, tr("Error"), tr("Error: need to implement merging because the slot you decided to invoke already has statements"));
                return false;
            }
            //We're definitely going to delete the slot, but let's copy it's statements (if any) to the actual dest
            int currentStatementIndex = 0;
            Q_FOREACH(IDesignEqualsImplementationStatement *currentStatement, destinationSlotIsProbablyNameless_OrZeroIfNoDest->orderedListOfStatements())
            {
                userChosenDestinationSlot_OrZeroIfNone->insertStatementIntoOrderedListOfStatements(currentStatementIndex++, currentStatement);
            }

            destinationClassLifeLine_OrZeroIfNoDest->removeSlotFromClassLifeLine(destinationSlotIsProbablyNameless_OrZeroIfNoDest);
            DesignEqualsImplementationClass *parentClass = destinationSlotIsProbablyNameless_OrZeroIfNoDest->ParentClass;
            parentClass->removeSlot(destinationSlotIsProbablyNameless_OrZeroIfNoDest);
            //parentClass->addSlot();
            int slotIndexOnClassLifeline_TriesToChooseIndexAfterWhatTheyTargetted = 0;
            if(destinationClassLifeLineGraphicsItem_OrZeroIfNoDest && destinationSlotIsProbablyNameless_OrZeroIfNoDest)
            {
                //use what they targeted + 1
                QList<DesignEqualsImplementationClassSlot*> slotsAppearingOnClassLifeLine = destinationClassLifeLineGraphicsItem_OrZeroIfNoDest->classLifeLine()->mySlotsAppearingInClassLifeLine();
                slotIndexOnClassLifeline_TriesToChooseIndexAfterWhatTheyTargetted = slotsAppearingOnClassLifeLine.indexOf(destinationSlotIsProbablyNameless_OrZeroIfNoDest);
                if(slotIndexOnClassLifeline_TriesToChooseIndexAfterWhatTheyTargetted < 0)
                {
                    slotIndexOnClassLifeline_TriesToChooseIndexAfterWhatTheyTargetted = slotsAppearingOnClassLifeLine.size();
                }
            }
            else if(destinationClassLifeLine_OrZeroIfNoDest)
            {
                //use 'last'
                slotIndexOnClassLifeline_TriesToChooseIndexAfterWhatTheyTargetted = destinationClassLifeLine_OrZeroIfNoDest->mySlotsAppearingInClassLifeLine().size();
            }
            destinationClassLifeLine_OrZeroIfNoDest->insertSlotToClassLifeLine(slotIndexOnClassLifeline_TriesToChooseIndexAfterWhatTheyTargetted, userChosenDestinationSlot_OrZeroIfNone);

            delete destinationSlotIsProbablyNameless_OrZeroIfNoDest;
            destinationSlotIsProbablyNameless_OrZeroIfNoDest = userChosenDestinationSlot_OrZeroIfNone;
        }

#if 0
        if(destinationSlotIsProbablyNameless_OrZeroIfNoDest->Name == "")
        {
            //TODOreq: delete targetSlotUnderGraphicsItemIsProbablyNameless_OrZeroIfNoDest under some circumstances?
            //delete targetSlotUnderGraphicsItemIsProbablyNameless_OrZeroIfNoDest;

            //userChosenDestinationSlot_OrZeroIfNone = targetSlotUnderGraphicsItemIsProbablyNameless_OrZeroIfNoDest;
            //targetSlotUnderGraphicsItemIsProbablyNameless_OrZeroIfNoDest = userChosenDestinationSlot_OrZeroIfNone;
            //targetSlotUnderGraphicsItemIsProbablyNameless_OrZeroIfNoDest->Name = userChosenDestinationSlot_OrZeroIfNone

            if(destinationClassLifeLine_OrZeroIfNoDest && destinationClassLifeLine_OrZeroIfNoDest->mySlotsAppearingInClassLifeLine().size() == 1) //TODOreq: need much better slot strategy, since it apparently happens every time...
            {
                DesignEqualsImplementationClassSlot *defaultSlotInClassLifeLine = destinationClassLifeLine_OrZeroIfNoDest->mySlotsAppearingInClassLifeLine().first();
                if(defaultSlotInClassLifeLine->Name == "")
                {
                    //destinationSlotParentClassLifeLine->removeSlotReference(defaultSlotInClassLifeLine);
                    //destinationSlotParentClassLifeLine->insertSlotReference(userChosenDestinationSlot_OrZeroIfNone);
                }

            }
        }
        else
        {
#if 0
            if(destinationClassLifeLine_OrZeroIfNoDest)
            {
                //TODOreq: since the target slot is already named, we need to create a new slot, or perhaps just put a slot reference in a class lifeline? i want a new visual slot!
                destinationClassLifeLine_OrZeroIfNoDest->insertSlotToClassLifeLine(420 /*TODOreq*/, userChosenDestinationSlot_OrZeroIfNone);
            }
#endif
        }
#endif
    }

    int classLifeLineIndexIntoUseCasesListOfClassLifeLines_OfDestinationSlot = -1;
    if(destinationClassLifeLine_OrZeroIfNoDest)
        classLifeLineIndexIntoUseCasesListOfClassLifeLines_OfDestinationSlot = m_UseCase->classLifeLines().indexOf(destinationClassLifeLine_OrZeroIfNoDest);

    if(userChosenDestinationSlot_OrZeroIfNone && userChosenSourceSignal_OrZeroIfNone)
    {
        //Signal/slot activation
        emit insertSignalSlotActivationUseCaseEventRequested(indexToInsertStatementAt_IntoSource, sourceSlotForStatementInsertion_OrZeroIfSourceIsActor, userChosenSourceSignal_OrZeroIfNone, userChosenDestinationSlot_OrZeroIfNone, signalEmissionOrSlotInvocationContextVariables, sourceClassLifeLine_OrZeroIfSourceIsActor, classLifeLineIndexIntoUseCasesListOfClassLifeLines_OfDestinationSlot);
        return true;
    }
    if(!userChosenSourceSignal_OrZeroIfNone && userChosenDestinationSlot_OrZeroIfNone)
    {
        //Slot invocation
        //TODOreq: is more in line with reactor pattern to delete/redraw line once backend adds the use case event. in any case:

        if(sourceIsActor)
        {
            emit setUseCaseSlotEntryPointRequested(destinationClassLifeLine_OrZeroIfNoDest, userChosenDestinationSlot_OrZeroIfNone);
            return true;
        }

        emit insertSlotInvocationUseCaseEventRequested(indexToInsertStatementAt_IntoSource, sourceSlotForStatementInsertion_OrZeroIfSourceIsActor, destinationClassLifeLine_OrZeroIfNoDest, userChosenDestinationSlot_OrZeroIfNone, signalEmissionOrSlotInvocationContextVariables); //TODOreq: makes sense that the unit of execution is emitted as well, but eh I'm kinda just tacking unit of execution on at this point and still don't see clearly how it fits in xD
        return true;
    }
    if(destinationIsActor && userChosenSourceSignal_OrZeroIfNone)
    {
        if(sourceItem)
        emit setExitSignalRequested(sourceSlotForStatementInsertion_OrZeroIfSourceIsActor, userChosenSourceSignal_OrZeroIfNone, signalEmissionOrSlotInvocationContextVariables);
        return true;
    }
    if(!userChosenDestinationSlot_OrZeroIfNone && userChosenSourceSignal_OrZeroIfNone)
    {
        //Signal with no listeners at time of design emit
        emit insertSignalEmissionUseCaseEventRequested(indexToInsertStatementAt_IntoSource, sourceSlotForStatementInsertion_OrZeroIfSourceIsActor, userChosenSourceSignal_OrZeroIfNone, signalEmissionOrSlotInvocationContextVariables);
        return true;
    }
    emit e("Error: Message editor dialog didn't give us anything we could work with");
    return false;
}
bool UseCaseGraphicsScene::itemIsWantedType(QGraphicsItem *itemToCheckIfWant, const QList<int> &listOfTypesTheyWant)
{
    int itemType = itemToCheckIfWant->type();
    Q_FOREACH(int currentWantedItemType, listOfTypesTheyWant)
    {
        if(itemType == currentWantedItemType)
            return true;
    }
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
    const QList<QGraphicsItem*> &itemsUnderPoint = items(pointToLookForItemsWeWant);
    Q_FOREACH(QGraphicsItem *currentItem, itemsUnderPoint)
    {
        if(itemIsWantedType(currentItem, m_ListOfItemTypesForArrowPressOrReleaseMode))
            return currentItem;
    }
    return 0;
}
QList<QGraphicsItem *> UseCaseGraphicsScene::itemsIWantIntersectingRect(QRectF rectWithinWhichToLookForItemsWeWant, const QList<int> &listOfTypesTheyWant)
{
    const QList<QGraphicsItem*> &itemsUnderPoint = items(rectWithinWhichToLookForItemsWeWant);
    QList<QGraphicsItem*> ret;

        //TODOreq: when the user clicks or releases for the name up top of the class lifeline (the class name itself), find most natural use case unit class lifeline of execution for the arrow connection
    Q_FOREACH(QGraphicsItem *currentItemWithinRect, itemsUnderPoint)
    {
        if(itemIsWantedType(currentItemWithinRect, listOfTypesTheyWant))
        {
            ret << currentItemWithinRect;
        }
    }
    return ret;
}
//Assumes not empty list
QGraphicsItem *UseCaseGraphicsScene::findNearestPointOnItemBoundingRectFromPoint(const QList<QGraphicsItem*> &itemsToCheck, QPointF pointToFindNearestEdge)
{
    QMap<qreal, QGraphicsItem*> distanceSorter;
    Q_FOREACH(QGraphicsItem *currentItem, itemsToCheck)
    {
        distanceSorter.insert(calculateDistanceFromPointToNearestPointOnBoundingRect(pointToFindNearestEdge, currentItem), currentItem);
    }
    return distanceSorter.first();
}
qreal UseCaseGraphicsScene::calculateDistanceFromPointToNearestPointOnBoundingRect(QPointF pointCalculateNearestEdgeTo, QGraphicsItem *item)
{
    QPointF nearestPointOnBoundingRectToSuppliedPoint = calculateNearestPointOnBoundingRectToArbitraryPoint(pointCalculateNearestEdgeTo, item);
    //Been a while since I used pythagoreom theorem
    qreal a = pointCalculateNearestEdgeTo.x() - nearestPointOnBoundingRectToSuppliedPoint.x();
    qreal b = pointCalculateNearestEdgeTo.y() - nearestPointOnBoundingRectToSuppliedPoint.y();
    qreal aSquared = a*a;
    qreal bSquared = b*b;
    qreal cSquared = aSquared+bSquared;
    return qSqrt(qAbs(cSquared));
}
QPointF UseCaseGraphicsScene::calculateNearestPointOnBoundingRectToArbitraryPoint(QPointF pointCalculateNearestEdgeTo, QGraphicsItem *item)
{
    const QRectF &itemBoundingRect = item->sceneBoundingRect();
    QPointF itemPos = item->scenePos();
    qreal widthDiv2 = (itemBoundingRect.width()/2);
    qreal heightDiv2 = (itemBoundingRect.height()/2);
    qreal itemLeftInScene = itemPos.x() - widthDiv2;
    qreal itemRightInScene = itemPos.x() + widthDiv2;
    qreal itemTopInScene = itemPos.y() - heightDiv2;
    qreal itemBottomInScene = itemPos.y() + heightDiv2;
    QPointF nearestPoint(
                //X:
                qMin(qMax(pointCalculateNearestEdgeTo.x(), itemLeftInScene), itemRightInScene),
                //Y:
                qMin(qMax(pointCalculateNearestEdgeTo.y(), itemTopInScene), itemBottomInScene)
                );
    return nearestPoint;
}
QRectF UseCaseGraphicsScene::mouseSnappingRect(QPointF mousePoint)
{
    QPointF topLeft(mousePoint.x()-(UseCaseGraphicsScene_MOUSE_HOVER_SQUARE_SIDE_LENGTH/2), mousePoint.y()-(UseCaseGraphicsScene_MOUSE_HOVER_SQUARE_SIDE_LENGTH/2));
    QPointF bottomRight(mousePoint.x()+(UseCaseGraphicsScene_MOUSE_HOVER_SQUARE_SIDE_LENGTH/2), mousePoint.y()+(UseCaseGraphicsScene_MOUSE_HOVER_SQUARE_SIDE_LENGTH/2));
    return QRectF(topLeft, bottomRight);
}
#if 0
DesignEqualsImplementationClassLifeLineUnitOfExecution* UseCaseGraphicsScene::targetUnitOfExecutionIfUnitofExecutionIsUnnamed_FirstAfterTargetIfNamedEvenIfYouHaveToCreateIt(DesignEqualsImplementationClassLifeLine *classLifeline)
{
    Q_FOREACH(DesignEqualsImplementationClassLifeLineUnitOfExecution *currentUnitOfExecution, classLifeline->m_UnitsOfExecution)
    {

    }
}
#endif
bool UseCaseGraphicsScene::wantDragDropEvent(QGraphicsSceneDragDropEvent *event)
{
    return (event->dropAction() == Qt::LinkAction && event->mimeData()->hasFormat(DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_USE_CASE_OBJECT));
}
void UseCaseGraphicsScene::handleActorAdded(DesignEqualsImplementationActor *actor)
{
    //TODOreq
    //TODOreq: don't add more than 1 actor (it can be deleted and re-added however)

    DesignEqualsImplementationActorGraphicsItemForUseCaseScene *actorGraphicsItem = new DesignEqualsImplementationActorGraphicsItemForUseCaseScene(actor);
    actorGraphicsItem->setPos(actor->position());
    addItem(actorGraphicsItem);
}
void UseCaseGraphicsScene::handleClassLifeLineAdded(DesignEqualsImplementationClassLifeLine *newClassLifeLine)
{
    DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *designEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene = new DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(newClassLifeLine); //TODOreq: right-click -> change thread (main/gui thread (default), new thread X, existing thread Y). they should somehow visually indicate their thread. i was thinking a shared "filled-background" (color), but even if just the colors of the lifelines are the same that would be sufficient
    connect(designEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene, SIGNAL(slotGraphicsItemInsertedIntoClassLifeLineGraphicsItem(DesignEqualsImplementationSlotGraphicsItemForUseCaseScene*)), this, SLOT(handleSlotGraphicsItemInsertedIntoClassLifeLineGraphicsItem(DesignEqualsImplementationSlotGraphicsItemForUseCaseScene*)));

    designEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene->setPos(newClassLifeLine->position()); //TODOreq: listen for moves

    addItem(designEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene);
}
void UseCaseGraphicsScene::handleEventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventTypeEnum, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionOrSlotInvocationContextVariables)
{
    //TODOreq: draw the fucking use case event blah, make the arrows permanent i guess... maybe even change their shape if signal/slot... and let sender/receiver each be able to reference the arrow (but maybe that part is done in backend)
}
void UseCaseGraphicsScene::handleSlotGraphicsItemInsertedIntoClassLifeLineGraphicsItem(DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *slotGraphicsItem)
{
    if(m_MouseMode == DesignEqualsImplementationMouseDrawSignalSlotConnectionActivationArrowsMode && m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn)
    {
        QLineF existingLine = m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->line();
        QPointF slotJustAddedPos = slotGraphicsItem->scenePos();
        QPointF topLeftOfSlotJustAdded;
        QRectF boundingRectOfSlotJustAdded = slotGraphicsItem->boundingRect();
        topLeftOfSlotJustAdded.setX(slotJustAddedPos.x()-(boundingRectOfSlotJustAdded.width()/2));
        topLeftOfSlotJustAdded.setY(slotJustAddedPos.y()-(boundingRectOfSlotJustAdded.height()/2));
        existingLine.setP2(topLeftOfSlotJustAdded); //TODOreq: topRight when relevant
        m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn->setLine(existingLine);
    }
}
void UseCaseGraphicsScene::setMouseMode(DesignEqualsImplementationMouseModeEnum newMouseMode)
{
    m_MouseMode = newMouseMode;
}
