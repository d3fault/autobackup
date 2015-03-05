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
#include "designequalsimplementationclasslifelineslotgraphicsitemforusecasescene.h"
#include "signalslotconnectionactivationarrowforgraphicsscene.h"
#include "designequalsimplementationexistinsignalgraphicsitemforusecasescene.h"
#include "designequalsimplementationslotinvokegraphicsitemforusecasescene.h"
#include "designequalsimplementationprivatemethodinvokestatementgraphicsitemforusecasescene.h"
#include "designequalsimplementationchunkofrawcppstatementsgraphicsitemforusecasescene.h"
#include "signalstatementnotchmultiplextergraphicsrect.h"
#include "signalslotmessagedialog.h"
#include "snappingindicationvisualrepresentation.h"
#include "classinstancechooserdialog.h"
#include "../../designequalsimplementationproject.h"
#include "../../designequalsimplementationclass.h"
#include "../../designequalsimplementationactor.h"
#include "../../designequalsimplementationclasslifeline.h"
#include "../../designequalsimplementationsignalemissionstatement.h"
#include "../../designequalsimplementationslotinvocationstatement.h"
#include "../../designequalsimplementationchunkofrawcppstatements.h"

#define UseCaseGraphicsScene_MOUSE_HOVER_SQUARE_SIDE_LENGTH 25

//TODOreq: if I put 2x Foos in the scene, and connected one to the other, wouldn't that be an infinite loop? Don't allow that if yes
//TODOreq: moving an item should make arrows move with it
// this struct calls "myCustomDeallocator" to delete the pointer
//OT: slotInvoke is used when a signal isn't warranted design-wise. The designer could be wrong or change their mind later and convert it to a signal/slot use case event, perhaps and probably after asking/prying from other developers. Still, pure slotInvokes help keep the API to a minimum (invoker not littered with corresponding [auto-generated] signals)
//TODOoptional: if dest hasA source (example: Foo hasA Bar), then we could perhaps have an "auto-generate handler (handleBarSignal) on dest" checkbox pre-checked" (there is no pushbutton, only this checkbox)... BECAUSE it is impossible (well, in this case at least) for Bar to invokeMethod whatever foo slot they choose. We KNOW they're going to want a slot, because that handler is now part of the use case / design
//TODOoptional: it makes sense that the "first slot" is not existing yet (but there's still the problems of how to add statements to it!).... what I'm getting at is that there should always be a "+1" slot/unit-of-execution ready for accepting arrow connections. creating it on mouse RELEASE is kinda ugly/hacky. when you have no stuf, all you see is that +1. After you've added stuff, at the bottom of every lifeline should be another pseudo-empty "+1" slot. My JIT create/destroy stuff would mostly stay the same (it must exist in order to add statements to it), BUT basically I'm just saying the GUI should lie about how many slots there are. When there are zero, it should show 1 (but still problem of connecting statements to "nameless" (not yet "invoked") slot, guh. I've flip flopped on this issue like a million times over the last few days, but I finally think the cleanest solution is to have a "ghost +1" to connect to. My existing "connect to slot and then JIT create one" is an ugly user-experience (kinda ;-P). Actually since typing that "kinda" I'm leaning back towards the other way ROFL. Because the user does not care about slots or units of execution!!! Showing a "ghost +1" might just confuse them ("where do I connect the line?")
//TODOreq: if the first item placed is actor, we should scroll/reposition graphics view so that actor is in top-left corner. we should maybe do the same for the first class lifeline, but maybe leaving some space to the left for actor/other?
//TODOreq: when deserializing: applies to both graphics scenes: the first item placed goes to 0,0 -- regardless of what coords I specify (this is just how the graphics scene works). So what that means is that every item coords after the first item must be translated to become relative to the first item's coords.
//TODOoptional: [de-]serailized "bookmarked" items (each use case has a list? or one list project-wide?). double clicking simply centers that object in the graphics view. there should additionally be an "all objects" parrallel list that is double click-able as well, but maintaining bookmarked ones allows the user to prioritize arbitrarily
//TODOreq: I guess there's the concept of signal references also. That is to say, if a signal is emitted in multiple places in the same use case, we should NOT (rite?) draw the slots that are attached to it, and should somehow visually indicate that it's emitting a signal that was connected "above"/elsewhere. The signal listeners are the same of course. Eventually I think I'll need to even add connect/disconnect STATEMENTS, but for now the connections are in the constructor and for the lifetime of the objects
//TODoreq: remember the last used mouse mode when returning to use cases view (tab range). move mode is going to be especially useless once i implement some auto layout scheme, and i might remove move mode altogether (maybe it will still be available in class diagram mode, maybe it won't)
//TODOreq: not only do I need to make slot invocations touch the upper-left hand corner of the slot graphics item, but additionally when it was INSERTED (either as a statement or a slot on existing signal), I possibly (if same class lifeline, etc) need to re-order the slots on the classlifeline to account for the INSERT. Additionally, maybe the ordering of the slots stays the same but maybe in some auto-layout mode I would still need for a slot graphics item to slide up or down on it's class lifeline graphics item based on the result of either of those kinds of INSERTS as well, simply because they change the visual geometry
//TODoreq: signal (aka "no dest") click-drag from existing signal statement [graphics item] shows the dialog but slot is unselectable and therefore dialog un-accept-able. Better to not show dialog at all (in the future I might allow signal to signal daisy chaining)
QGraphicsItem *UseCaseGraphicsScene::createVisualRepresentationBasedOnStatementType(IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements *theStatement, int indexInsertedInto, DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *parent)
{
    switch(theStatement->StatementType)
    {
    case IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements::SignalEmitStatementType:
    {
        DesignEqualsImplementationSignalEmissionStatement *signalEmitStatement = static_cast<DesignEqualsImplementationSignalEmissionStatement*>(theStatement);
        DesignEqualsImplementationClassLifeLine *sourceClassLifeline = parent->parentClassLifelineGraphicsItem()->classLifeLine();
        DesignEqualsImplementationClassSlot *slotThatSignalWasEmittedFrom = parent->underlyingSlot();
        DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *sourceSlotGraphicsItem = parent;
        return new DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene(sourceSlotGraphicsItem->parentClassLifelineGraphicsItem()->parentUseCaseGraphicsScene(), sourceClassLifeline, slotThatSignalWasEmittedFrom, indexInsertedInto, signalEmitStatement->signalToEmit(), parent);
    }
        break;
    case IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements::SlotInvokeStatementType:
    {
        DesignEqualsImplementationSlotInvocationStatement *slotInvokeStatement = static_cast<DesignEqualsImplementationSlotInvocationStatement*>(theStatement);
        DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *sourceSlotGraphicsItem = parent;
        return new DesignEqualsImplementationSlotInvokeGraphicsItemForUseCaseScene(sourceSlotGraphicsItem->parentClassLifelineGraphicsItem()->parentUseCaseGraphicsScene(), slotInvokeStatement->classLifelineWhoseSlotIsToBeInvoked(), slotInvokeStatement->slotToInvoke(), parent);
    }
        break;
    case IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements::PrivateMethodSynchronousCallStatementType:
        return new DesignEqualsImplementationPrivateMethodInvokeStatementGraphicsItemForUseCaseScene(parent);
        break;
    case IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements::ChunkOfRawCppStatements:
        return new DesignEqualsImplementationChunkOfRawCppStatementsGraphicsItemForUseCaseScene(static_cast<DesignEqualsImplementationChunkOfRawCppStatements*>(theStatement), parent);
        break;
        //TODOreq: etc
    }
    return 0;
}
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
DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *UseCaseGraphicsScene::classLifelineGraphicsItemByClassLifeline_OrZeroIfNotFound(DesignEqualsImplementationClassLifeLine *classLifelineToRetrieveGraphicsItemFor)
{
    Q_FOREACH(DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *currentClassLifelineGraphicsItem, m_ClassLifelineGraphicsItemsInUseCaseGraphicsScene)
    {
        if(currentClassLifelineGraphicsItem->classLifeLine() == classLifelineToRetrieveGraphicsItemFor)
            return currentClassLifelineGraphicsItem;
    }
    return 0;
}
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
                m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone = new SignalSlotConnectionActivationArrowForGraphicsScene(m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone->itemProxyingFor(), m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone->insertIndexForProxyItem(), QLineF(m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone->visualRepresentation()->scenePos(), event->scenePos()), m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone->optionalSubInsertIndexForWhenConnectingSlotToExistingSignal_OrNegativeOneIfNotApplicable());
                addItem(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone);
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
                    int optionalSubIndexForSlotOnExistingSignalSlotConnectionList = -1;
                    if(itemUnderMouse->type() == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassSlot_GRAPHICS_TYPE_ID) //TODOreq: do the same for existing signal + notch, and also remember the sub-index!
                    {
                        indexOnUnitOfExecutionThatStatementIsInsertedInto = static_cast<DesignEqualsImplementationSlotGraphicsItemForUseCaseScene*>(itemUnderMouse)->getInsertIndexForMouseScenePos(mouseEventScenePos);
                    }
                    else if(itemUnderMouse->type() == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ExistingSignal_GRAPHICS_TYPE_ID)
                    {
                        DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene *existingSignalStatementGraphicsItem = static_cast<DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene*>(itemUnderMouse);
                        indexOnUnitOfExecutionThatStatementIsInsertedInto = existingSignalStatementGraphicsItem->indexStatementInsertedInto();
                        optionalSubIndexForSlotOnExistingSignalSlotConnectionList = existingSignalStatementGraphicsItem->getInsertSubIndexForMouseScenePos(mouseEventScenePos);
                    }
                    else if(itemUnderMouse->type() == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ExistingSignal_SignalStatementNotchMultiplexterGraphicsRect_GRAPHICS_TYPE_ID)
                    {
                        SignalStatementNotchMultiplexterGraphicsRect *signalNotchMultiplexerGraphicsItem = static_cast<SignalStatementNotchMultiplexterGraphicsRect*>(itemUnderMouse);
                        indexOnUnitOfExecutionThatStatementIsInsertedInto = signalNotchMultiplexerGraphicsItem->parentSignalStatementGraphicsItem()->indexStatementInsertedInto();
                        optionalSubIndexForSlotOnExistingSignalSlotConnectionList = signalNotchMultiplexerGraphicsItem->getInsertSubIndexForMouseScenePos(mouseEventScenePos);
                    }
                    m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone = new SignalSlotConnectionActivationArrowForGraphicsScene(itemUnderMouse, indexOnUnitOfExecutionThatStatementIsInsertedInto, QLineF(mouseEventScenePos, mouseEventScenePos), optionalSubIndexForSlotOnExistingSignalSlotConnectionList);
                    addItem(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone);

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
    if(m_MouseMode == DesignEqualsImplementationMouseDrawSignalSlotConnectionActivationArrowsMode && !m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone)
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

#if 0 //TODOoptional: fix inheritence stuffz, simplifies what is immediately below, but does same thing
            ISnappableSourceGraphicsItem *snappableSourceGraphicsItem = (ISnappableSourceGraphicsItem*) /*hack*/ /*static_cast<ISnappableSourceGraphicsItem*>(*/itemWithEdgeNearestToPoint/*)*/; //for now we rely on findNearestPointOnItemBoundingRectFromPoint only returning graphics items that inherit from ISnappableGraphicsItem. Every entry in m_ListOfItemTypesIWant_SnapSource must inherit from ISnappableGraphicsItem. We could do stronger type checking (making sure it's in m_ListOfItemTypesIWant_SnapSource, but that would be the second time I checked it so nah..
#endif
            ISnappableSourceGraphicsItem *snappableSourceGraphicsItem;
            switch(itemWithEdgeNearestToPoint->type())
            {
                case DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassSlot_GRAPHICS_TYPE_ID:
                    snappableSourceGraphicsItem = static_cast<DesignEqualsImplementationSlotGraphicsItemForUseCaseScene*>(itemWithEdgeNearestToPoint);
                    break;
                case DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ExistingSignal_GRAPHICS_TYPE_ID:
                    snappableSourceGraphicsItem = static_cast<DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene*>(itemWithEdgeNearestToPoint);
                    break;
            case DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ExistingSignal_SignalStatementNotchMultiplexterGraphicsRect_GRAPHICS_TYPE_ID:
                    snappableSourceGraphicsItem = static_cast<SignalStatementNotchMultiplexterGraphicsRect*>(itemWithEdgeNearestToPoint);
                    break;
            default:
                snappableSourceGraphicsItem = 0;
                break;
            }

            if(m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone)
            {
                delete m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone;
                m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone = 0;
            }

            if(snappableSourceGraphicsItem)
                m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone = snappableSourceGraphicsItem->makeSnappingHelperForMousePoint(eventScenePos);
        }
        else if(m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone)
        {
            delete m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone;
            m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone = 0;
        }
    }

    if(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone)
    {
        QLineF newLine(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone->line().p1(), event->scenePos());
        m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone->setLine(newLine);

        //Destination snapping

        //TODOoptional: only allow the left OR right side to be allowed for destination snapping ("O"), depending on which direction the source is relative to us

        QPointF eventScenePos = event->scenePos();
        QRectF myMouseSnappingRect = mouseSnappingRect(event->scenePos());
        QList<QGraphicsItem*> itemsNearMouse = itemsIWantIntersectingRect(myMouseSnappingRect, m_ListOfItemTypesIWant_SnapDestination);
        if(!itemsNearMouse.isEmpty())
        {
            //Find closest
            QGraphicsItem *itemWithEdgeNearestToPoint = findNearestPointOnItemBoundingRectFromPoint(itemsNearMouse, eventScenePos);

            if(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone->sourceGraphicsItem() == itemWithEdgeNearestToPoint)
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
    if(m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone)
    {
        //if(!prosessMouseReleaseEvent_andReturnWhetherOrNotToKeepTempArrowForThisMouseReleaseEvent(event))
        processMouseReleaseEvent_andReturnWhetherOrNotToKeepTempArrowForThisMouseReleaseEvent(event);
        {
            delete m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone; //TODOreq: either always delete, or pass to backend to at least "add"/update for reactor pattern. Backend creates new units of execution (whenever target is slot) that the arrow must point to, but the unit of execution to point to doesn't exist until the backend tells us via signal
        }
        m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone = 0;
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
DesignEqualsImplementationUseCase *UseCaseGraphicsScene::useCase() const
{
    return m_UseCase;
}
void UseCaseGraphicsScene::privateConstructor(DesignEqualsImplementationUseCase *useCase)
{
    m_UseCase = useCase;
    m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone = 0;
    m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone = 0;
    m_ArrowDestinationSnapper_OrZeroIfNone = 0;

    //TODOoptimization: make these lists static, no need for each use case to have a separate identical list
    m_ListOfItemTypesForArrowPressOrReleaseMode.append(DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassLifeLine_GRAPHICS_TYPE_ID);
    m_ListOfItemTypesForArrowPressOrReleaseMode.append(DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassSlot_GRAPHICS_TYPE_ID);
    m_ListOfItemTypesForArrowPressOrReleaseMode.append(DesignEqualsImplementationActorGraphicsItemForUseCaseScene_Actor_GRAPHICS_TYPE_ID);
    m_ListOfItemTypesForArrowPressOrReleaseMode.append(DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ExistingSignal_GRAPHICS_TYPE_ID); //TODOreq: am I sure I want this for release mode ALSO? I guess I could proxy to the slot the existing signal is emitted from, but it makes more sense to only have in the list for mouse PRESS
    m_ListOfItemTypesForArrowPressOrReleaseMode.append(DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ExistingSignal_SignalStatementNotchMultiplexterGraphicsRect_GRAPHICS_TYPE_ID);

    //m_ListOfItemTypesIWant_SnapSource.append(DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassLifeLine_GRAPHICS_TYPE_ID);
    m_ListOfItemTypesIWant_SnapSource.append(DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassSlot_GRAPHICS_TYPE_ID);
    m_ListOfItemTypesIWant_SnapSource.append(DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ExistingSignal_GRAPHICS_TYPE_ID);
    m_ListOfItemTypesIWant_SnapSource.append(DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ExistingSignal_SignalStatementNotchMultiplexterGraphicsRect_GRAPHICS_TYPE_ID);

    //m_ListOfItemTypesIWant_SnapDestination.append(DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassLifeLine_GRAPHICS_TYPE_ID);
    m_ListOfItemTypesIWant_SnapDestination.append(DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassSlot_GRAPHICS_TYPE_ID);

    //TODOreq: snap to/from actor


    //requests
    connect(this, SIGNAL(addActorToUseCaseRequsted(QPointF)), useCase, SLOT(addActorToUseCase(QPointF)));
    connect(this, SIGNAL(addClassToUseCaseRequested(DesignEqualsImplementationClass*,/*TODOinstancing: DesignEqualsImplementationClassInstance*,*/QPointF)), useCase, SLOT(createClassLifelineInUseCase(DesignEqualsImplementationClass*,/*TODOinstancing: DesignEqualsImplementationClassInstance*,*/QPointF)));

    connect(this, SIGNAL(insertSlotInvocationUseCaseEventRequested(int,IDesignEqualsImplementationHaveOrderedListOfStatements*,DesignEqualsImplementationClassLifeLine*,DesignEqualsImplementationClassSlot*,SignalEmissionOrSlotInvocationContextVariables)), useCase, SLOT(insertSlotInvocationEvent(int,IDesignEqualsImplementationHaveOrderedListOfStatements*,DesignEqualsImplementationClassLifeLine*,DesignEqualsImplementationClassSlot*,SignalEmissionOrSlotInvocationContextVariables)));
    connect(this, SIGNAL(insertSignalSlotActivationUseCaseEventRequested(int,DesignEqualsImplementationClassSlot*,DesignEqualsImplementationClassSignal*,DesignEqualsImplementationClassSlot*,SignalEmissionOrSlotInvocationContextVariables,DesignEqualsImplementationClassLifeLine*,int)), useCase, SLOT(insertSignalSlotActivationEvent(int,DesignEqualsImplementationClassSlot*,DesignEqualsImplementationClassSignal*,DesignEqualsImplementationClassSlot*,SignalEmissionOrSlotInvocationContextVariables,DesignEqualsImplementationClassLifeLine*,int)));
    connect(this, SIGNAL(insertSignalEmissionUseCaseEventRequested(int,IDesignEqualsImplementationHaveOrderedListOfStatements*,DesignEqualsImplementationClassSignal*,SignalEmissionOrSlotInvocationContextVariables,DesignEqualsImplementationClassLifeLine*)), useCase, SLOT(insertSignalEmitEvent(int,IDesignEqualsImplementationHaveOrderedListOfStatements*,DesignEqualsImplementationClassSignal*,SignalEmissionOrSlotInvocationContextVariables,DesignEqualsImplementationClassLifeLine*)));
    connect(this, SIGNAL(insertExistingSignalNewSlotEventRequested(int,DesignEqualsImplementationClassSlot*,DesignEqualsImplementationClassSignal*,DesignEqualsImplementationClassSlot*,SignalEmissionOrSlotInvocationContextVariables,DesignEqualsImplementationClassLifeLine*,int,int)), useCase, SLOT(insertExistingSignalNewSlotEvent(int,DesignEqualsImplementationClassSlot*,DesignEqualsImplementationClassSignal*,DesignEqualsImplementationClassSlot*,SignalEmissionOrSlotInvocationContextVariables,DesignEqualsImplementationClassLifeLine*,int,int)));

    connect(this, SIGNAL(setUseCaseSlotEntryPointRequested(DesignEqualsImplementationClassLifeLine*,DesignEqualsImplementationClassSlot*)), useCase, SLOT(setUseCaseSlotEntryPoint(DesignEqualsImplementationClassLifeLine*,DesignEqualsImplementationClassSlot*)));
    connect(this, SIGNAL(setExitSignalRequested(DesignEqualsImplementationClassSlot*,DesignEqualsImplementationClassSignal*,SignalEmissionOrSlotInvocationContextVariables)), useCase, SLOT(setExitSignal(DesignEqualsImplementationClassSlot*,DesignEqualsImplementationClassSignal*,SignalEmissionOrSlotInvocationContextVariables)));

    //responses
    connect(useCase, SIGNAL(actorAdded(DesignEqualsImplementationActor*)), this, SLOT(handleActorAdded(DesignEqualsImplementationActor*)));
    connect(useCase, SIGNAL(classLifeLineAdded(DesignEqualsImplementationClassLifeLine*)), this, SLOT(handleClassLifeLineAdded(DesignEqualsImplementationClassLifeLine*)));
    connect(useCase, SIGNAL(signalEmitEventAdded(int,IDesignEqualsImplementationHaveOrderedListOfStatements*,DesignEqualsImplementationClassSignal*,int)), this, SLOT(handleSignalEmitEventAdded(int,IDesignEqualsImplementationHaveOrderedListOfStatements*,DesignEqualsImplementationClassSignal*,int)));
    connect(useCase, SIGNAL(slotAddedToExistingSignalSlotConnectionList(DesignEqualsImplementationClassSignal*,DesignEqualsImplementationClassLifeLine*,DesignEqualsImplementationClassSlot*,int)), this, SLOT(handleSlotAddedToExistingSignalSlotConnectionList(DesignEqualsImplementationClassSignal*,DesignEqualsImplementationClassLifeLine*,DesignEqualsImplementationClassSlot*,int)));
    //connect(useCase, SIGNAL(eventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum,QObject*,SignalEmissionOrSlotInvocationContextVariables)), this, SLOT(handleEventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum,QObject*,SignalEmissionOrSlotInvocationContextVariables)));

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
bool UseCaseGraphicsScene::processMouseReleaseEvent_andReturnWhetherOrNotToKeepTempArrowForThisMouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //first things first, get the topmost items [that we want] underneath the source and dest points
    QGraphicsItem *sourceItem = m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone->sourceGraphicsItem(); //it was already set to zero if no items determined wanted
    if(!sourceItem)
        return false; //for now, we require a source
    QGraphicsItem *destinationItem_CanBeZeroUnlessSourceIsActor = 0;
    if(m_ArrowDestinationSnapper_OrZeroIfNone)
    {
        destinationItem_CanBeZeroUnlessSourceIsActor = m_ArrowDestinationSnapper_OrZeroIfNone->itemProxyingFor();
        QLineF existingLine = m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone->line();
        existingLine.setP2(m_ArrowDestinationSnapper_OrZeroIfNone->visualRepresentation()->scenePos());
        m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone->setLine(existingLine);
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
                if(destinationSlotIsProbablyNameless_OrZeroIfNoDest->Name.startsWith(UseCaseGraphicsScene_TEMP_SLOT_MAGICAL_NAME_STRING_PREFIX))
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
                    DesignEqualsImplementationClass *classToCreateSlotIn = destinationSlotIsProbablyNameless_OrZeroIfNoDest->ParentClass;
                    DesignEqualsImplementationClassSlot *newSlot = classToCreateSlotIn->createwNewSlot(classToCreateSlotIn->nextTempUnnamedSlotName());
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
        else if(sourceItemType == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ExistingSignal_GRAPHICS_TYPE_ID)
        {
            DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene *existingSignalGraphicsItem = static_cast<DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene*>(sourceItem);
            sourceSlotForStatementInsertion_OrZeroIfSourceIsActor = existingSignalGraphicsItem->slotThatSignalWasEmittedFrom();
            sourceClassLifeLine_OrZeroIfSourceIsActor = existingSignalGraphicsItem->sourceClassLifeline();
        }
        else if(sourceItemType == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ExistingSignal_SignalStatementNotchMultiplexterGraphicsRect_GRAPHICS_TYPE_ID)
        {
            SignalStatementNotchMultiplexterGraphicsRect *notchMultiplexerGraphicsItem = static_cast<SignalStatementNotchMultiplexterGraphicsRect*>(sourceItem);
            DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene *existingSignalGraphicsItem = notchMultiplexerGraphicsItem->parentSignalStatementGraphicsItem();
            sourceSlotForStatementInsertion_OrZeroIfSourceIsActor = existingSignalGraphicsItem->slotThatSignalWasEmittedFrom();
            sourceClassLifeLine_OrZeroIfSourceIsActor = existingSignalGraphicsItem->sourceClassLifeline();
        }
    }

    if(sourceSlotForStatementInsertion_OrZeroIfSourceIsActor == destinationSlotIsProbablyNameless_OrZeroIfNoDest)
        return false; //Drag arrow onto self = unsupported for now. TODOoptional: call private methods this way (with arch arrow)?

    DesignEqualsImplementationClassSignal *sourceExistingSignalStatement_OrZeroIfSourceIsNotExistingSignalStatement = 0;
    if(!sourceIsActor)
    {
        if(sourceItemType == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ExistingSignal_GRAPHICS_TYPE_ID)
        {
            DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene *sourceExistingSignalStatementGraphicsItem = static_cast<DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene*>(sourceItem);
            sourceExistingSignalStatement_OrZeroIfSourceIsNotExistingSignalStatement = sourceExistingSignalStatementGraphicsItem->underlyingSignal();
        }
        else if(sourceItemType == DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ExistingSignal_SignalStatementNotchMultiplexterGraphicsRect_GRAPHICS_TYPE_ID)
        {
            SignalStatementNotchMultiplexterGraphicsRect *sourceExistingSignalStatementSlotNotchMultiplexerGraphicsRect = static_cast<SignalStatementNotchMultiplexterGraphicsRect*>(sourceItem);
            sourceExistingSignalStatement_OrZeroIfSourceIsNotExistingSignalStatement = sourceExistingSignalStatementSlotNotchMultiplexerGraphicsRect->parentSignalStatementGraphicsItem()->underlyingSignal();
        }
    }

    //TODOreq: by/at-around now, we know which index in the source the statement should be inserted at. We don't have to give it to the dialog, but we do need to emit it to the backend after dialog is accepted
    int indexToInsertStatementAt_IntoSource = m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone->statementInsertIndex();

    SignalSlotMessageDialog signalSlotMessageCreatorDialog(messageEditorDialogMode, destinationSlotIsProbablyNameless_OrZeroIfNoDest, sourceIsActor, destinationIsActor, sourceClassLifeLine_OrZeroIfSourceIsActor, destinationClassLifeLine_OrZeroIfNoDest, sourceSlotForStatementInsertion_OrZeroIfSourceIsActor, sourceExistingSignalStatement_OrZeroIfSourceIsNotExistingSignalStatement); //TODOreq: segfault if drawing line to anything other than unit of execution lololol. TODOreq: i have 3 options, idk which makes the most sense: pass in unit of execution, pass in class lifeline, or pass i class. perhaps it doesn't matter... but for now to play it safe i'll pass in the unit of execution, since he has a reference to the other two :-P
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
        if(userChosenDestinationSlot_OrZeroIfNone != destinationSlotIsProbablyNameless_OrZeroIfNoDest && destinationSlotIsProbablyNameless_OrZeroIfNoDest->Name.startsWith(UseCaseGraphicsScene_TEMP_SLOT_MAGICAL_NAME_STRING_PREFIX) && destinationClassLifeLine_OrZeroIfNoDest)
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
            Q_FOREACH(IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements *currentStatement, destinationSlotIsProbablyNameless_OrZeroIfNoDest->orderedListOfStatements())
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
        if(signalSlotMessageCreatorDialog.signalIsExistingSignalFlag())
        {
            //Existing signal statement, new slot
            emit insertExistingSignalNewSlotEventRequested(indexToInsertStatementAt_IntoSource, sourceSlotForStatementInsertion_OrZeroIfSourceIsActor, userChosenSourceSignal_OrZeroIfNone, userChosenDestinationSlot_OrZeroIfNone, signalEmissionOrSlotInvocationContextVariables, sourceClassLifeLine_OrZeroIfSourceIsActor, classLifeLineIndexIntoUseCasesListOfClassLifeLines_OfDestinationSlot, m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone->optionalSubInsertIndexForWhenConnectingSlotToExistingSignal_OrNegativeOneIfNotApplicable());  //TODOreq: ensure all are actually valid. TODOoptimization: some of the args are probably not used and can be ommitted. but actually the method called might require them anyways lol so meh
            return true;
        }

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
        emit insertSignalEmissionUseCaseEventRequested(indexToInsertStatementAt_IntoSource, sourceSlotForStatementInsertion_OrZeroIfSourceIsActor, userChosenSourceSignal_OrZeroIfNone, signalEmissionOrSlotInvocationContextVariables, sourceClassLifeLine_OrZeroIfSourceIsActor);
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
QPointF UseCaseGraphicsScene::calculatePointOnSlotOnClassLifelineThatWeUseAsAStartPoint_Aka_P1_ifWeWereALine_UsingTheIndexThatTheStatementWasInsertedInto(int indexInto_m_ClassLifeLines_OfSignal, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceSlot, DesignEqualsImplementationClassSignal *signalUseCaseEvent, int indexStatementWasInsertedInto)
{

    DesignEqualsImplementationClassLifeLine *signalClassLifeline = m_UseCase->classLifeLines().at(indexInto_m_ClassLifeLines_OfSignal);

#if 0
    IDesignEqualsImplementationStatement *maybeSourceSlot = 0;
    Q_FOREACH(IDesignEqualsImplementationStatement *currentSlotTesting, signalClassLifeline->mySlotsAppearingInClassLifeLine())
    {
        if(currentSlotTesting == sourceSlot)
        {
            maybeSourceSlot = currentSlotTesting;
            break;
        }
    }
#if 0
    if(!maybeSourceSlot)
    {
        //qFatal("couldn't calculate point on slot on class lifeline because we couldn't find the indicated slot on that class lifeline");
        return QPointF(0,0);
    }
#endif
    Q_ASSERT(maybeSourceSlot != 0);
#endif



    DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *maybeClassLifeline = 0;
    Q_FOREACH(DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *currentClassLifelineGraphicsItem, m_ClassLifelineGraphicsItemsInUseCaseGraphicsScene)
    {
        if(currentClassLifelineGraphicsItem->classLifeLine() == signalClassLifeline)
        {
            maybeClassLifeline = currentClassLifelineGraphicsItem;
            break;
        }
    }
    Q_ASSERT(maybeClassLifeline != 0);

    DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *maybeSlotGraphicsItem = 0;
    Q_FOREACH(DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *currentSlotGraphicsItem, maybeClassLifeline->m_SlotGraphicsItemsInThisClassLifeLine)
    {
        if(currentSlotGraphicsItem->underlyingSlot() == sourceSlot)
        {
            maybeSlotGraphicsItem = currentSlotGraphicsItem;
            break;
        }
    }
    Q_ASSERT(maybeSlotGraphicsItem != 0);

    return maybeSlotGraphicsItem->calculatePointForStatementsP1AtStatementIndex(indexStatementWasInsertedInto);

    //signalStatementContainingBodyListOfStatements
}
#endif
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
    DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *designEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene = new DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(this, newClassLifeLine); //TODOreq: right-click -> change thread (main/gui thread (default), new thread X, existing thread Y). they should somehow visually indicate their thread. i was thinking a shared "filled-background" (color), but even if just the colors of the lifelines are the same that would be sufficient
    connect(designEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene, SIGNAL(slotGraphicsItemInsertedIntoClassLifeLineGraphicsItem(DesignEqualsImplementationSlotGraphicsItemForUseCaseScene*)), this, SLOT(handleSlotGraphicsItemInsertedIntoClassLifeLineGraphicsItem(DesignEqualsImplementationSlotGraphicsItemForUseCaseScene*)));

    designEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene->setPos(newClassLifeLine->position()); //TODOreq: listen for moves

    m_ClassLifelineGraphicsItemsInUseCaseGraphicsScene.append(designEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene);

    addItem(designEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene);
}
void UseCaseGraphicsScene::handleSignalEmitEventAdded(int indexInto_m_ClassLifeLines_OfSignal, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceSlot, DesignEqualsImplementationClassSignal *signalUseCaseEvent, int indexStatementWasInsertedInto)
{
    //TODOreq: anything?
    /*
     DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene *signalGraphicsItem = new DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene();
    addItem(signalGraphicsItem);
    signalGraphicsItem->setPos(calculatePointOnSlotOnClassLifelineThatWeUseAsAStartPoint_Aka_P1_ifWeWereALine_UsingTheIndexThatTheStatementWasInsertedInto(indexInto_m_ClassLifeLines_OfSignal, sourceSlot, indexStatementWasInsertedInto));
    */
}
void UseCaseGraphicsScene::handleSlotAddedToExistingSignalSlotConnectionList(DesignEqualsImplementationClassSignal *existingSignalSlotWasAddedTo, DesignEqualsImplementationClassLifeLine *sourceClassLifeline, DesignEqualsImplementationClassSlot *sourceSlotTheSignalWasEmittedFrom, int signalEmitStatementIndexInSlotEmittedFrom)
{
    DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *sourceClassLifelineGraphicsItem = classLifelineGraphicsItemByClassLifeline_OrZeroIfNotFound(sourceClassLifeline);
    if(!sourceClassLifelineGraphicsItem)
        return; //should never happen
    DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *sourceSlotGraphicsItemTheSignalWasEmittedFrom = sourceClassLifelineGraphicsItem->slotGraphicsItemForSlot_OrZeroIfNotFound(sourceSlotTheSignalWasEmittedFrom);
    if(!sourceSlotGraphicsItemTheSignalWasEmittedFrom)
        return; //should never happen
    ExistingStatementListEntryTypedef existingSignalStatementAndGraphicsItem = sourceSlotGraphicsItemTheSignalWasEmittedFrom->existingStatementsAndTheirGraphicsItems().at(signalEmitStatementIndexInSlotEmittedFrom);
    if(existingSignalStatementAndGraphicsItem.second->StatementType != IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements::SignalEmitStatementType)
        return; //should never happen
    DesignEqualsImplementationSignalEmissionStatement *signalEmitStatement = static_cast<DesignEqualsImplementationSignalEmissionStatement*>(existingSignalStatementAndGraphicsItem.second);
    if(signalEmitStatement->signalToEmit() != existingSignalSlotWasAddedTo)
        return; //should never happen
    if(existingSignalStatementAndGraphicsItem.first->type() != DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ExistingSignal_GRAPHICS_TYPE_ID)
        return; //should never happen
    DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene *existingSignalGraphicsItem = static_cast<DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene*>(existingSignalStatementAndGraphicsItem.first);
    existingSignalGraphicsItem->redoVisualStuffz0rz(); //to account for new slot
}
#if 0 //the comment inside is still worth reading
void UseCaseGraphicsScene::handleEventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventTypeEnum, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionOrSlotInvocationContextVariables)
{
    //TODOreq: draw the fucking use case event blah, make the arrows permanent i guess... maybe even change their shape if signal/slot... and let sender/receiver each be able to reference the arrow (but maybe that part is done in backend)
}
#endif
void UseCaseGraphicsScene::handleSlotGraphicsItemInsertedIntoClassLifeLineGraphicsItem(DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *slotGraphicsItem)
{
    if(m_MouseMode == DesignEqualsImplementationMouseDrawSignalSlotConnectionActivationArrowsMode && m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone)
    {
        QLineF existingLine = m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone->line();
        QPointF slotJustAddedPos = slotGraphicsItem->scenePos();
        QPointF topLeftOfSlotJustAdded;
        QRectF boundingRectOfSlotJustAdded = slotGraphicsItem->boundingRect();
        topLeftOfSlotJustAdded.setX(slotJustAddedPos.x()-(boundingRectOfSlotJustAdded.width()/2));
        topLeftOfSlotJustAdded.setY(slotJustAddedPos.y()-(boundingRectOfSlotJustAdded.height()/2));
        existingLine.setP2(topLeftOfSlotJustAdded); //TODOreq: topRight when relevant
        m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone->setLine(existingLine);
    }
}
void UseCaseGraphicsScene::setMouseMode(DesignEqualsImplementationMouseModeEnum newMouseMode)
{
    m_MouseMode = newMouseMode;
}
