#include "designequalsimplementationclasslifelineslotgraphicsitemforusecasescene.h"

#include <QLineF>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

#include "designequalsimplementationguicommon.h"
#include "sourceslotsnappingindicationvisualrepresentation.h"
#include "destinationslotsnappingindicationvisualrepresentation.h"
#include "usecasegraphicsscene.h"
#include "designequalsimplementationclasslifelinegraphicsitemforusecasescene.h"
#include "designequalsimplementationexistinsignalgraphicsitemforusecasescene.h"
#include "../../designequalsimplementationproject.h"
#include "../../designequalsimplementationclass.h"
#include "../../designequalsimplementationclassslot.h"
#include "../../idesignequalsimplementationstatement.h"
#include "../../designequalsimplementationcommon.h"

#define DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE 25

//TODOoptional: the statement graphics items could be rects and the statement 'text' could go inside the rects. does actually save [vertical] real-estate, since we now only are a little bit taller than the text
//TODOoptional: if the slot is unnamed and there are more than zero statements attached, we could (should?) show a ghost slot invocation graphic. we could also use a valid auto-generated C++ slot name. The name is chosen when the slot is placed in class lifeline, but we don't show it until there are > 0 statements because the normal flow of creation has them invoke the slot from something "from the left" and naming it during that connecting stage (Showing them the auto-named "ghost" slot invocation" at all times is confusing. However, showing it when there are > 0 statements serves as a reminder (but the fact that it is a valid C++ CLASS name (TODOreq: use slot name in auto-generated slot name) that they are supposed to connect it from the left (supposed to name it))
DesignEqualsImplementationSlotGraphicsItemForUseCaseScene::DesignEqualsImplementationSlotGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifeLine, DesignEqualsImplementationClassSlot *slot, QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
    , m_ParentClassLifeline(parentClassLifeLine)
    , m_Slot(slot)
    //, m_UnitOfExecution(unitOfExecution)
{
    privateConstructor();
}
DesignEqualsImplementationSlotGraphicsItemForUseCaseScene::DesignEqualsImplementationSlotGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifeLine, DesignEqualsImplementationClassSlot *slot, const QRectF &rect, QGraphicsItem *parent)
    : QGraphicsRectItem(rect, parent)
    , m_ParentClassLifeline(parentClassLifeLine)
    , m_Slot(slot)
    //, m_UnitOfExecution(slot)
{
    privateConstructor();
}
DesignEqualsImplementationSlotGraphicsItemForUseCaseScene::DesignEqualsImplementationSlotGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifeLine, DesignEqualsImplementationClassSlot *slot, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent)
    : QGraphicsRectItem(x, y, w, h, parent)
    , m_ParentClassLifeline(parentClassLifeLine)
    , m_Slot(slot)
    //, m_UnitOfExecution(slot)
{
    privateConstructor();
}
int DesignEqualsImplementationSlotGraphicsItemForUseCaseScene::getInsertIndexForMouseScenePos(QPointF mouseEventScenePos)
{
    //some copy/pasta from makeSnappingHelperForMousePoint
    QPointF mouseItemPos = mapFromScene(mouseEventScenePos);
    QMap<qreal, int> distancesFromMousePointAndTheirCorrespondingIndexesInOurInternalList_Sorter;
    Q_FOREACH(qreal currentVerticalPositionOfSnapPoint, m_VerticalPositionsOfSnapPoints)
    {
        distancesFromMousePointAndTheirCorrespondingIndexesInOurInternalList_Sorter.insert(qAbs(currentVerticalPositionOfSnapPoint - mouseItemPos.y()), currentVerticalPositionOfSnapPoint);
    }
    if(!distancesFromMousePointAndTheirCorrespondingIndexesInOurInternalList_Sorter.isEmpty())
    {
        return distancesFromMousePointAndTheirCorrespondingIndexesInOurInternalList_Sorter.first();
    }
    return 0;
}
IRepresentSnapGraphicsItemAndProxyGraphicsItem *DesignEqualsImplementationSlotGraphicsItemForUseCaseScene::makeSnappingHelperForSlotEntryPoint(QPointF mouseScenePosForDeterminingRightOrLeftOnly)
{
    QPointF mouseItemPos = mapFromScene(mouseScenePosForDeterminingRightOrLeftOnly);
    bool left = mouseItemPos.x() < 0;
    qreal x;
    if(left)
    {
        x = rect().left();
    }
    else
    {
        x = rect().right();
    }
    qreal y = rect().top();
    IRepresentSnapGraphicsItemAndProxyGraphicsItem *snappingIndicationVisualRepresentation = new DestinationSlotSnappingIndicationVisualRepresentation(this, 0, this);
    snappingIndicationVisualRepresentation->visualRepresentation()->setPos(x, y);
    return snappingIndicationVisualRepresentation;
}
IRepresentSnapGraphicsItemAndProxyGraphicsItem *DesignEqualsImplementationSlotGraphicsItemForUseCaseScene::makeSnappingHelperForMousePoint(QPointF mouseScenePos)
{
    //Find nearest BEFORE, BETWEEN, or AFTER position, and tell using visuals that it snaps
    QPointF mouseItemPos = mapFromScene(mouseScenePos);
    bool left = mouseItemPos.x() < 0;

    //NOPE BECAUSE STILL DON'T KNOW "nearest", would be inaccurate between final two: An easy way to find the nearest is to just start the odd/even loop, then see if the vertical distance is growing or shrinking. If growing, stop and use the last one. If shrinking, continue until
    //QPointF myItemMousePos = mapFromParent(mouseItemPos);

    QMap<qreal /*distance*/, QPair<qreal /*vertical height*/, int /*index into vertical height array*/> > distancesFromMousePointAndTheirCorrespondingVerticalHeightsInOurInternalList_Sorter;
    int currentIndex = 0;
    Q_FOREACH(qreal currentVerticalPositionOfSnapPoint, m_VerticalPositionsOfSnapPoints)
    {
        distancesFromMousePointAndTheirCorrespondingVerticalHeightsInOurInternalList_Sorter.insert(qAbs(currentVerticalPositionOfSnapPoint - mouseItemPos.y()), qMakePair(currentVerticalPositionOfSnapPoint, currentIndex++));
    }
    if(!distancesFromMousePointAndTheirCorrespondingVerticalHeightsInOurInternalList_Sorter.isEmpty())
    {
        qreal closestSnappingPointsYValue = distancesFromMousePointAndTheirCorrespondingVerticalHeightsInOurInternalList_Sorter.first().first;

        //TODOreq: how to manage the snapping visual item's lifetime
        //if(currentSnappingVisualIsAlreadyUsingA_Y_ValueOf(closestSnappingPointsYValue))
           //return;

        //reposition instead? who owns the snapping indication visual (line at the time of writing), me or the graphics scene?

        IRepresentSnapGraphicsItemAndProxyGraphicsItem *sourceSnappingIndicationVisualRepresentation = new SourceSlotSnappingIndicationVisualRepresentation(this, distancesFromMousePointAndTheirCorrespondingVerticalHeightsInOurInternalList_Sorter.first().second, this);
        sourceSnappingIndicationVisualRepresentation->visualRepresentation()->setPos((left ? rect().left() : rect().right()), closestSnappingPointsYValue);
        return sourceSnappingIndicationVisualRepresentation;
    }
    return 0;
}
DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *DesignEqualsImplementationSlotGraphicsItemForUseCaseScene::parentClassLifelineGraphicsItem() const
{
    return m_ParentClassLifeline;
}
DesignEqualsImplementationClassSlot *DesignEqualsImplementationSlotGraphicsItemForUseCaseScene::underlyingSlot() const
{
    return m_Slot;
}
int DesignEqualsImplementationSlotGraphicsItemForUseCaseScene::type() const
{
    return DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassSlot_GRAPHICS_TYPE_ID;
}
QList<ExistingStatementListEntryTypedef> DesignEqualsImplementationSlotGraphicsItemForUseCaseScene::existingStatementsAndTheirGraphicsItems() const
{
    return m_ExistingStatementsAndTheirGraphicsItems;
}
void DesignEqualsImplementationSlotGraphicsItemForUseCaseScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    //TODOmb: double clicking slot graphics item could open c++ edit mode?

    QMenu menu;
    QAction *editCppAction = menu.addAction(tr("Edit C++"));
    QAction *removeSlotFromClassLifelineAction = menu.addAction(tr("Remove Slot From Class Lifeline"));
    QAction *selectedAction = menu.exec(event->screenPos());
    if(!selectedAction)
        return;
    if(selectedAction == editCppAction)
    {
        emit editCppModeRequested(m_Slot->ParentClass, m_Slot);
    }
    if(selectedAction == removeSlotFromClassLifelineAction)
    {
        //TODOreq
    }
}
#if 0
QPointF DesignEqualsImplementationSlotGraphicsItemForUseCaseScene::calculatePointForStatementsP1AtStatementIndex(int statementIndexToCalculateP1for)
{

}
#endif
const QRectF DesignEqualsImplementationSlotGraphicsItemForUseCaseScene::minRect()
{
    static const QRect minRect(-(DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_HALF_WIDTH), -(DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_MINIMUM_VERTICAL_SIZE/2), (DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_HALF_WIDTH*2), DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_MINIMUM_VERTICAL_SIZE);
    return minRect;
}
void DesignEqualsImplementationSlotGraphicsItemForUseCaseScene::privateConstructor()
{
    QString myToolTip = m_Slot->ParentClass->Name + "::";
    if(m_Slot->Name.startsWith(UseCaseGraphicsScene_TEMP_SLOT_MAGICAL_NAME_STRING_PREFIX))
    {
#ifdef QT_DEBUG
            myToolTip.append(m_Slot->Name);
#else //Release
            myToolTip.append("(unnamed slot)");
#endif // QT_DEBUG
    }
    else
    {
        myToolTip.append(m_Slot->methodSignatureWithoutReturnType());
    }
    setToolTip(myToolTip); //TODOreq: update tooltip slot rename

    //Draw existing (serialized) statements, or at least make the vertical space for them and know they exist (as of writing, "arrows" are unfinished)
    //I think for now I'm not going to have any visual representation of the "amount of statements", I'll just use a simple odd/even strategy for "statements" vs. "snapping points"
    connect(m_Slot, SIGNAL(statementInserted(int,IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements*)), this, SLOT(handleStatementInserted(int,IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements*)));

    if(!m_Slot->orderedListOfStatements().isEmpty())
    {
        int currentStatemendIndex = 0;
        Q_FOREACH(IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements *currentStatement, m_Slot->orderedListOfStatements())
        {
            insertStatementGraphicsItem(currentStatemendIndex++, currentStatement);
        }
    }
    else //no statements, so use minRect as rect
    {
        setRect(minRect());
    }
    repositionExistingStatementsAndSnapPoints(); //regardless of any statements, we always want at least 1 snap point to be positioned

    connect(this, SIGNAL(editCppModeRequested(DesignEqualsImplementationClass*,DesignEqualsImplementationClassSlot*)), m_Slot->ParentClass->m_ParentProject, SLOT(handleEditCppModeRequested(DesignEqualsImplementationClass*,DesignEqualsImplementationClassSlot*)));
}
void DesignEqualsImplementationSlotGraphicsItemForUseCaseScene::insertStatementGraphicsItem(int indexInsertedInto, IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements *statementInserted)
{
    QGraphicsItem *newVisualRepresentationOfStatement = UseCaseGraphicsScene::createVisualRepresentationBasedOnStatementType(statementInserted, indexInsertedInto, this);
    //newVisualRepresentationOfStatement->setPos(QPointF(DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_HALF_WIDTH, 0));
    //newVisualRepresentationOfStatement->moveBy(50, 0);
    m_ExistingStatementsAndTheirGraphicsItems.insert(indexInsertedInto, qMakePair(newVisualRepresentationOfStatement, statementInserted));
}
void DesignEqualsImplementationSlotGraphicsItemForUseCaseScene::repositionExistingStatementsAndSnapPoints()
{
    //TODOoptimization: "append"/etc doesn't reposition ALL of them

    int numStatementsOrSnapPointsInUnitOfExecution = ((m_ExistingStatementsAndTheirGraphicsItems.size()*2)+1); //Times two accounts for all our snap OR insert positions, Plus one accounts for "insert before or insert after"

    qreal maybeRectHeight = calculateMyRectHeightUsingExistingStatements();
#if 0
    qreal maybeRectHeight = ((numStatementsOrSnapPointsInUnitOfExecution+1)*DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE);
#endif
    qreal myRectHeight = qMax(minRect().height(), maybeRectHeight);

    QRectF myRect = minRect();
    myRect.setHeight(myRectHeight);
    myRect.moveTop(-myRect.height()/2);
    myRect.moveLeft(-myRect.width()/2);
    setRect(myRect);

    qreal currentVerticalPos = myRect.top() + DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE;

    m_VerticalPositionsOfSnapPoints.clear();
    bool even = true;
    //int loopEnd = (numVerticalDistancesInUnitOfExecution+1);  //don't start at zero because we don't care about edges (loop end accounts for this with the +1)
    int existingStatementsIndex = 0;
    for(int i = 0; i < numStatementsOrSnapPointsInUnitOfExecution; ++i)
    {
        //qreal currentY = static_cast<qreal>(i) * static_cast<qreal>(DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE);
        if(even)
        {
            //even

            m_VerticalPositionsOfSnapPoints.append(currentVerticalPos);

            even = false;
        }
        else
        {
            //odd

            m_ExistingStatementsAndTheirGraphicsItems.at(existingStatementsIndex++).first->setPos(0, currentVerticalPos);

            even = true;
        }

        currentVerticalPos += DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE;
    }
    if(m_VerticalPositionsOfSnapPoints.isEmpty())
    {
        //Minimum 1 snap point at all times
        m_VerticalPositionsOfSnapPoints.append(myRectHeight/2);
    }
#if 0
    QRectF myRect = minRect();
    qreal newRectHeightMaybe = 0; //visauls included
    //QPointF runningStatementCenterPosition(0, 0); //statement/snap model only
    m_VerticalPositionsOfSnapPoints.clear(); //might be handy when factoring in inserts (these points need recalculating)
    if(m_UnitOfExecution->methodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution()) //named? should be by now, but just to avoid a segfault...
    {
        newRectHeightMaybe += DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_STATEMENT_MARGIN_TO_EDGE; //accounting for the y margin added at top

        int numStatementOrSnapPositions = (m_ExistingStatements.size()*2)+1;
        //Q_FOREACH(ExistingStatementListEntryTypedef existingStatementListEntry, m_ExistingStatements)
        bool even = true;
        int existintStatementIndex = 0;
        for(int i = 0; i < numStatementOrSnapPositions; ++i)
        {
            //no mutex needed since it's our [copy of] the list and we don't need to access the "second" of the pair (underlying statement)
            if(!even) //Odd amount of pixels, put the current statement graphic
            {
                even = true;

                //new QGraphicsLineItem(this);(QLineF(leftOfLine, rightOfLine), this); //setPos? so confused about the difference...

                m_ExistingStatements.at(existintStatementIndex).first->setPos(QPointF(0, newRectHeightMaybe)); //TODOreq: bring this halfway closer to rightOfLine
                //m_ExistingStatements.at(existintStatementIndex).first->setLine(QLineF(leftOfLine, rightOfLine));
            }
            else //Even amount of pixels, mark it internally as a snapping point
            {
                even = false;
                m_VerticalPositionsOfSnapPoints.append(newRectHeightMaybe);
            }
            newRectHeightMaybe += DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE;
        }
        newRectHeightMaybe += DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE; //y margin at the bottom
    }
    if(newRectHeightMaybe > DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_MINIMUM_VERTICAL_SIZE)
    {
        //myRect.setHeight(newRectHeightMaybe);

#if 0
        //recenter. AWW SHIT the lines already there aren't recentered ffff
        //Am I missing something? Isn't there any easier way to center the content?
        QPointF topLeft(-(minRect.width()/2), -(newRectHeightMaybe/2));
        QPointF bottomRight(+(minRect.width()/2), (newRectHeightMaybe/2));
        QRectF myRect(topLeft, bottomRight);
#endif

        QRectF childrenRectStartingPoint = childrenBoundingRect(); //Fuck yea children bounding rect!
        childrenRectStartingPoint.setWidth(myRect.width());
        childrenRectStartingPoint.setHeight(qMax(newRectHeightMaybe, childrenRectStartingPoint.height()));

        QPointF topLeft(-(childrenRectStartingPoint.width()/2), -(childrenRectStartingPoint.height()/2));
        QPointF bottomRight((childrenRectStartingPoint.width()/2), (childrenRectStartingPoint.height()/2));
        QRectF myRect(topLeft, bottomRight);
    }
    //myRect.setTop(0);
    setRect(myRect);
#endif
}
qreal DesignEqualsImplementationSlotGraphicsItemForUseCaseScene::calculateMyRectHeightUsingExistingStatements()
{
    qreal myHeight = 0.0; //minheight stuff is still checked against the result of this method, so we only need to worry about accuracy
    myHeight += DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE; //top border/margin (we don't want to snap to top edge, nor do we want a statement to come out of top edge)

    int currentStatementIndex = 0;
    Q_FOREACH(ExistingStatementListEntryTypedef currentExistingStatement, m_ExistingStatementsAndTheirGraphicsItems)
    {
        if(currentExistingStatement.second->isSignalEmit())
        {
            myHeight += (DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE/2.0); //first halft
            myHeight += static_cast<DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene*>(currentExistingStatement.first)->calculateHeightPossiblyIncludingNotchMultiplexer(); //"additional" height maybe added on to account for slots
            myHeight += (DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE/2.0); //second half
#if 0
            QList<SlotConnectedToSignalTypedef> slotsConnectedToSignal = m_ParentClassLifeline->parentUseCaseGraphicsScene()->useCase()->slotsConnectedToSignal(m_ParentClassLifeline, m_Slot, currentStatementIndex);
            if(slotsConnectedToSignal.isEmpty())
            {
                myHeight += DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE;
            }
            else
            {
                //since we're calculating TOTAL HEIGHT in this method, we add the total height of the slot notch multiplexer. TODOreq: when calculating where slot statement should be touching on the emitting slot, we should use half the slot notch multplexer height
                myHeight += static_cast<DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene*>(currentExistingStatement.first)->notchMultiplexerRect_OrZeroIfNoSlotsAttachedToSignal()->boundingRect().height();
            }
#endif
        }
        else if(currentExistingStatement.second->isSlotInvoke())
        {
            myHeight += DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE;
        }
        else
        {
            //TODOreq: change to use private method call's graphical height once created, "loop back half circle calling self". for now going to take "0" [additional] height just like a slot invoke statement or signal emit with no slots attached
            myHeight += DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE;
        }
    }

    myHeight += DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE; //bottom border/margin (we don't want to snap to bottom edge, nor do we want a statement to come out of bottom edge)
    return myHeight;
}
void DesignEqualsImplementationSlotGraphicsItemForUseCaseScene::handleStatementInserted(int indexInsertedInto, IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements *statementInserted)
{
    insertStatementGraphicsItem(indexInsertedInto, statementInserted);
    repositionExistingStatementsAndSnapPoints();
    emit geometryChanged(); //parent class lifeline listens to this and repositions Units Of Execution. TODOreq: should i emit this when adding a new slot to an existing signal? technically it will still indirectly increase the height of the slot ("unit of executioni" block), so it kind of makes sense that yes i should
    //parentClassLifeline()->repositionUnitsOfExecution(); //TODOoptioinal: friend unit of execution from within class lifeline (??? idfk whether or not i should, so i won't (using that logic, every method should be public (wait nvm)))


#if 0 //maybe not needed here. maybe 'emit geometryChanged()' is good enough
    //and now fix positioning of all of the statement's "receivers" (only slot invocations at time of writing (TODOreq: signal with no slots is still used in "width calculation" for the class lifeline "to the right", regardless of if it is even connected to from any slots on our class lifeline)
    //TODOreq: ensure all statement receivers (slots) are ordered properly on their corresponding class lifelines, and are slid down enough but as high as possible
    Q_FOREACH(ExistingStatementListEntryTypedef existinStatementAndGraphicsItem, m_ExistingStatementsAndTheirGraphicsItems)
    {
        if(existinStatementAndGraphicsItem.second->isSignalEmit())
        {
            //signal width + (longest slot width that is connecting to class lifeline directly to the right, OR, if nomin class lifeline horizontal spacing
        }
        else if(existinStatementAndGraphicsItem.second->isSlotInvoke())
        {
            //only use width of slot in calculation
        }
        //TODOreq: still use width of private method call statement for spacing to next class lifeline 'to the right'?
    }
    //I've got the above foreach pseudo'd, but don't I need to account for every slot on this class lifeline, not just every statement on the current slot? Uhh one problem I have is that statements, and their snap points, won't any longer be even divides in the slot height. I need to account for the the fact that signals with > 0 slots attached take up more-than-usual vertical space. So the statement (and statement snap point) directly below any signal with > 0 slots attached will be lower than if a regular slot invoke (or signal emit with NO slots attached) were above it. I think positioning should start at class lifeline and work down and recursively "to the right"
#endif
}
