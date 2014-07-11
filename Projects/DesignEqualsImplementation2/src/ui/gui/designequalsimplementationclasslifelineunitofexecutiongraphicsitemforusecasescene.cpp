#include "designequalsimplementationclasslifelineunitofexecutiongraphicsitemforusecasescene.h"

#include <QLineF>

#include <QMutexLocker>
#include "../../designequalsimplementation.h"

#include "designequalsimplementationguicommon.h"
#include "../../designequalsimplementationclasslifelineunitofexecution.h"

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
void DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::showSnappingHelperForMousePoint(QPointF mouseScenePos)
{
    //Find nearest BEFORE, BETWEEN, or AFTER position, and tell using visuals that it snaps
    QPointF mouseItemPos = mapFromScene(mouseScenePos);
    bool left = mouseItemPos.x() < 0;

    //NOPE BECAUSE STILL DON'T KNOW "nearest", would be inaccurate between final two: An easy way to find the nearest is to just start the odd/even loop, then see if the vertical distance is growing or shrinking. If growing, stop and use the last one. If shrinking, continue until
    QPointF myItemMousePos = mapFromParent(mouseItemPos);
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
void DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::privateConstructor()
{
    //Draw existing (serialized) statements, or at least make the vertical space for them and know they exist (as of writing, "arrows" are unfinished)
    //I think for now I'm not going to have any visual representation of the "amount of statements", I'll just use a simple odd/even strategy for "statements" vs. "snapping points"
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);

    QRect minRect(-(DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_HALF_WIDTH), -(DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_MINIMUM_VERTICAL_SIZE/2), (DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_HALF_WIDTH*2), DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_MINIMUM_VERTICAL_SIZE);
    qreal newRectHeightMaybe = 0;
    QPointF leftOfLine(minRect.topLeft());
    leftOfLine.setX(leftOfLine.x()+DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_STATEMENT_MARGIN_TO_EDGE);
    leftOfLine.setY(leftOfLine.y()+DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_STATEMENT_MARGIN_TO_EDGE);
    QPointF rightOfLine(minRect.topRight());
    rightOfLine.setX(rightOfLine.x()-DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_STATEMENT_MARGIN_TO_EDGE);
    rightOfLine.setY(leftOfLine.y());
    newRectHeightMaybe += DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_STATEMENT_MARGIN_TO_EDGE; //accounting for the y margin added at top
    int currentVerticalOddEvenIndex = 0;
    if(m_UnitOfExecution->MethodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution) //named?
    {
        Q_FOREACH(IDesignEqualsImplementationStatement *currentStatement, m_UnitOfExecution->MethodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution->OrderedListOfStatements) //we might just need the count, but iterating it is one way to do it...
        {
            if((currentVerticalOddEvenIndex++) % 2 == 1) //Every odd amount of pixels, put the current statement graphic
            {
                new QGraphicsLineItem(QLineF(leftOfLine, rightOfLine), this); //setPos? so confused about the difference...
            }
            leftOfLine.setY(leftOfLine.y()+DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE);
            rightOfLine.setY(leftOfLine.y());
        }
    }
    newRectHeightMaybe += DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene_SNAP_OR_STATEMENT_VERTICAL_DISTANCE; //y margin at the bottom

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
        childrenRectStartingPoint.setWidth(minRect.width());
        childrenRectStartingPoint.setHeight(qMax(newRectHeightMaybe, childrenRectStartingPoint.height()));

        QPointF topLeft(-(childrenRectStartingPoint.width()/2), -(childrenRectStartingPoint.height()/2));
        QPointF bottomRight((childrenRectStartingPoint.width()/2), (childrenRectStartingPoint.height()/2));
        QRectF myRect(topLeft, bottomRight);

        setRect(myRect);
    }
    else
    {
        setRect(minRect);
    }
}
