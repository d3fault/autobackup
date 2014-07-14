#include "designequalsimplementationclasslifelineunitofexecutiongraphicsitemforusecasescene.h"

#include <QLineF>

#include <QMutexLocker>
#include "../../designequalsimplementation.h"

#include "designequalsimplementationguicommon.h"
#include "snappingindicationvisualrepresentation.h"
#include "designequalsimplementationclasslifelinegraphicsitemforusecasescene.h"
#include "../../designequalsimplementationclasslifelineunitofexecution.h"
#include "../../idesignequalsimplementationstatement.h"

#define DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE 25
#define DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_STATEMENT_MARGIN_TO_EDGE 10 //For the side opposite the arrow comes out

DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution, DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifeline, QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
    , m_UnitOfExecution(unitOfExecution)
    , m_ParentClassLifeline(parentClassLifeline)
{
    privateConstructor();
}
DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution, DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifeline, const QRectF &rect, QGraphicsItem *parent)
    : QGraphicsRectItem(rect, parent)
    , m_UnitOfExecution(unitOfExecution)
    , m_ParentClassLifeline(parentClassLifeline)
{
    privateConstructor();
}
DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution, DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifeline, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent)
    : QGraphicsRectItem(x, y, w, h, parent)
    , m_UnitOfExecution(unitOfExecution)
    , m_ParentClassLifeline(parentClassLifeline)
{
    privateConstructor();
}
int DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::getInsertIndexForMouseScenePos(QPointF mouseEventScenePos)
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
QGraphicsItem *DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::makeSnappingHelperForMousePoint(QPointF mouseScenePos)
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

        QGraphicsItem *snappingIndicationVisualRepresentation = new SnappingIndicationVisualRepresentation(this, distancesFromMousePointAndTheirCorrespondingVerticalHeightsInOurInternalList_Sorter.first().second, this);
        snappingIndicationVisualRepresentation->setPos((left ? rect().left() : rect().right()), closestSnappingPointsYValue);
        return snappingIndicationVisualRepresentation;
    }
    return 0;
}
DesignEqualsImplementationClassLifeLineUnitOfExecution *DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::unitOfExecution() const
{
    return m_UnitOfExecution;
}
DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::parentClassLifeline() const
{
    return m_ParentClassLifeline;
}
int DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::type() const
{
    return DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassLifeLineUnitOfExecution_GRAPHICS_TYPE_ID;
}
const QRectF DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::minRect()
{
    static const QRect minRect(-(DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_HALF_WIDTH), -(DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_MINIMUM_VERTICAL_SIZE/2), (DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_HALF_WIDTH*2), DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_MINIMUM_VERTICAL_SIZE);
    return minRect;
}
void DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::privateConstructor()
{
    //Draw existing (serialized) statements, or at least make the vertical space for them and know they exist (as of writing, "arrows" are unfinished)
    //I think for now I'm not going to have any visual representation of the "amount of statements", I'll just use a simple odd/even strategy for "statements" vs. "snapping points"
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
        connect(m_UnitOfExecution, SIGNAL(statementInserted(int,IDesignEqualsImplementationStatement*)), this, SLOT(handleStatementInserted(int,IDesignEqualsImplementationStatement*)));

    if(m_UnitOfExecution->methodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution()) //named?
    {
        int currentStatemendIndex = 0;
        Q_FOREACH(IDesignEqualsImplementationStatement *currentStatement, m_UnitOfExecution->methodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution()->orderedListOfStatements())
        {
            insertStatement(currentStatemendIndex, currentStatement);
        }
        repositionExistingStatementsAndSnapPoints();
    }
    else //not named, therefore empty. so set minRect as rect
    {
        setRect(minRect());
    }
}
void DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::insertStatement(int indexInsertedInto, IDesignEqualsImplementationStatement *statementInserted)
{
    QGraphicsLineItem *newGraphicsLineItemForVisualRepresentation = new QGraphicsLineItem(this);
    qreal statementHalfLength /*width sorta*/ = (minRect().width()-(DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_STATEMENT_MARGIN_TO_EDGE*2))/2; //the times two accounts for the margins on both left and right sides
    newGraphicsLineItemForVisualRepresentation->setLine(QLineF(QPointF(-statementHalfLength, 0), QPointF(statementHalfLength, 0)));
    //TODOreq: newGraphicsLineItemForVisualRepresentation->setLine( <- just make it go left and right along y=0, using whatever width to determine the two x points
    m_ExistingStatements.insert(indexInsertedInto, qMakePair(newGraphicsLineItemForVisualRepresentation, statementInserted));
}
void DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::repositionExistingStatementsAndSnapPoints()
{
    //TODOoptimization: "append"/etc doesn't reposition ALL of them

    int numStatementsOrSnapPointsInUnitOfExecution = ((m_ExistingStatements.size()*2)+1); //Times two accounts for all our snap/insert positions, First plus one accounts for "insert before and insert after"

    qreal maybeRectHeight = ((numStatementsOrSnapPointsInUnitOfExecution+1)*DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE);
    qreal myRectHeight = qMax(minRect().height(), maybeRectHeight);

    QRectF myRect = minRect();
    myRect.setHeight(myRectHeight);
    myRect.moveTop(-myRect.height()/2);
    myRect.moveLeft(-myRect.width()/2);
    setRect(myRect);

    qreal currentVerticalPos = myRect.top() + DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE;

    m_VerticalPositionsOfSnapPoints.clear();
    if(m_UnitOfExecution->methodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution()) //named? should be by now, but just to avoid a segfault...
    {
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

                m_ExistingStatements.at(existingStatementsIndex++).first->setPos(0, currentVerticalPos);

                even = true;
            }

            currentVerticalPos += DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE;
        }
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
void DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::handleStatementInserted(int indexInsertedInto, IDesignEqualsImplementationStatement *statementInserted)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    insertStatement(indexInsertedInto, statementInserted);
    repositionExistingStatementsAndSnapPoints();
    parentClassLifeline()->repositionUnitsOfExecution(); //TODOoptioinal: friend unit of execution from within class lifeline (??? idfk whether or not i should, so i won't (using that logic, every method should be public (wait nvm)))
}
