#include "designequalsimplementationclasslifelinegraphicsitemforusecasescene.h"

#include <QPainter>
#include <QLineF>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>

#include <QMutexLocker>
#include "../../designequalsimplementation.h"

#include "designequalsimplementationguicommon.h"
#include "designequalsimplementationclasslifelineunitofexecutiongraphicsitemforusecasescene.h"
#include "../../designequalsimplementationclass.h"
#include "../../designequalsimplementationclasslifeline.h"

#define DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_NO_ACTIVITY_LIFELINE_MINIMUM_VERTICAL_GAP 15

//TODOreq: if a slot invoked on another thread (to the right) invokes us (via signal or whatever) by drawing an arrow back to that same unit of execution, a new unit of execution is auto-created right then and there for it
//TODOmb: right-click -> new unit of execution (would be nameless, just like the very first one added)
DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLine *classLifeLine, QObject *qobjectParent, QGraphicsItem *graphicsItemParent)
    : QObject(qobjectParent)
    , QGraphicsRectItem(graphicsItemParent)
{
    privateConstructor(classLifeLine);
}
DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLine *classLifeLine, const QRectF &rect, QObject *qobjectParent, QGraphicsItem *graphicsItemParent)
    : QObject(qobjectParent)
    , QGraphicsRectItem(rect, graphicsItemParent)
{
    privateConstructor(classLifeLine);
}
DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLine *classLifeLine, qreal x, qreal y, qreal w, qreal h, QObject *qobjectParent, QGraphicsItem *graphicsItemParent)
    : QObject(qobjectParent)
    , QGraphicsRectItem(x, y, w, h, graphicsItemParent)
{
    privateConstructor(classLifeLine);
}
DesignEqualsImplementationClassLifeLine *DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::classLifeLine() const
{
    return m_DesignEqualsImplementationClassLifeLine;
}
int DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::type() const
{
    return DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassLifeLine_GRAPHICS_TYPE_ID;
}
void DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::privateConstructor(DesignEqualsImplementationClassLifeLine *classLifeLine)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);

    m_DesignEqualsImplementationClassLifeLine = classLifeLine;
    setFlag(QGraphicsItem::ItemIsMovable, true);
    //setFlag(QGraphicsItem::ItemIsSelectable, true);
    m_ClassBorderPen.setWidth(2);

    QString lifeLineTitleHtml("<b>"); //TODOreq: only the ClassName/Type should be bold
    if(classLifeLine->myInstanceInClassThatHasMe_OrZeroIfTopLevelObject())
    {
        lifeLineTitleHtml += classLifeLine->myInstanceInClassThatHasMe_OrZeroIfTopLevelObject()->m_DesignEqualsImplementationClass->ClassName + "</b>" + "* " /*exception to the rule, I want the star to be close to the type and not the class that owns me*/ + classLifeLine->myInstanceInClassThatHasMe_OrZeroIfTopLevelObject()->m_DesignEqualsImplementationClassThatHasMe->ClassName + "::" + classLifeLine->myInstanceInClassThatHasMe_OrZeroIfTopLevelObject()->VariableName;
    }
    else
    {
        lifeLineTitleHtml += classLifeLine->designEqualsImplementationClass()->ClassName + "</b>";
    }
    QGraphicsTextItem *classNameTextItem = new QGraphicsTextItem(this);
    classNameTextItem->setHtml(lifeLineTitleHtml);

    QRectF childrenBoundingRectStartingPoint = childrenBoundingRect();
    QPointF myTopLeft(-(childrenBoundingRectStartingPoint.width()/2), -(childrenBoundingRectStartingPoint.height()/2));
    QPointF myBottomRight(childrenBoundingRectStartingPoint.width()/2, childrenBoundingRectStartingPoint.height()/2);
    setRect(QRectF(myTopLeft, myBottomRight));

    classNameTextItem->setPos(mapToParent(0, 0));

    int currentIndex = 0;
    Q_FOREACH(DesignEqualsImplementationClassLifeLineUnitOfExecution *currentUnitOfExecution, classLifeLine->unitsOfExecution())
    {
        insertUnitOfExecutionGraphicsItem(currentIndex++, currentUnitOfExecution);
        //new DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene(currentUnitOfExecution, this, unitOfExecutionRect, this);
    }
    repositionUnitsOfExecution();
    connect(classLifeLine, SIGNAL(unitOfExecutionInserted(int,DesignEqualsImplementationClassLifeLineUnitOfExecution*)), this, SLOT(handleUnitOfExecutionInserted(int,DesignEqualsImplementationClassLifeLineUnitOfExecution*)));
}
void DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::insertUnitOfExecutionGraphicsItem(int indexInsertedInto, DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution)
{
    m_DesignedOrderedButOnlySemiFlowOrderedUnitsOfExecution.insert(indexInsertedInto, new DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene(unitOfExecution, this, this));
}
//TODOreq: in the future the "lines in between" lengths should be user stretchable and serializable, but for now to KISS they'll be fixed length (primarily, the unit of execution rect itself will auto-move (which will indirectly lengthen the line above it) or stretch, but it does make sense for the user to put it right where they want it vertically
void DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::repositionUnitsOfExecution() //TODOoptimization: an append would be much easier/faster, but I should get the essentials out of the way first
{
    QPen dashedPen;
    dashedPen.setStyle(Qt::DotLine);
    while(m_LinesJustAboveEachUnitOfExecution.size() < m_DesignedOrderedButOnlySemiFlowOrderedUnitsOfExecution.size())
    {
        QGraphicsLineItem *newLineItem = new QGraphicsLineItem(this);
        m_LinesJustAboveEachUnitOfExecution.append(newLineItem);
        newLineItem->setPen(dashedPen);
    }

    QPointF topOfLifeLine, bottomOfLifeLine;
    topOfLifeLine.setX((rect().right()-rect().left())/2);
    bottomOfLifeLine.setX(topOfLifeLine.x());
    topOfLifeLine.setY(rect().bottom());
    bottomOfLifeLine.setY(topOfLifeLine.y()+DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_NO_ACTIVITY_LIFELINE_MINIMUM_VERTICAL_GAP);
    QLineF lineAboveUnitOfExecution(topOfLifeLine, bottomOfLifeLine);
    int currentLineIndex = 0;
    Q_FOREACH(DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene *currentUnitOfExecutionGraphicsItem, m_DesignedOrderedButOnlySemiFlowOrderedUnitsOfExecution)
    {
        m_LinesJustAboveEachUnitOfExecution.at(currentLineIndex++)->setLine(lineAboveUnitOfExecution); //TODOreq: drawing arrow to these dashed lines should behave just like drawing arrow to the class name box
        QPointF topLeftOfUnitOfExecutionRect(bottomOfLifeLine.x()-DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_HALF_WIDTH, bottomOfLifeLine.y());
        QPointF bottomRightOfUnitOfExecutionRect(bottomOfLifeLine.x()+DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_HALF_WIDTH, bottomOfLifeLine.y()+DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_MINIMUM_VERTICAL_SIZE);
        QRectF unitOfExecutionRect(topLeftOfUnitOfExecutionRect, bottomRightOfUnitOfExecutionRect);

        //currentUnitOfExecutionGraphicsItem->setRect(unitOfExecutionRect); //or don't I mean setPos, since I am the parent and he manages his size?
        QPointF topMiddlePos(0, bottomOfLifeLine.y());
        currentUnitOfExecutionGraphicsItem->setPos(topMiddlePos);

        //get next line in place, even if there aren't any more units of execution
        topOfLifeLine.setY(unitOfExecutionRect.bottom());
        bottomOfLifeLine.setY(topOfLifeLine.y()+DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_NO_ACTIVITY_LIFELINE_MINIMUM_VERTICAL_GAP);
        lineAboveUnitOfExecution.setPoints(topOfLifeLine, bottomOfLifeLine);
    }
}
void DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::handleUnitOfExecutionInserted(int indexInsertedInto, DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution)
{
    //make the graphics item, add it as child, and reposition (NOPE: for now just append, but i might need layouts or something in order to do inserts)
    insertUnitOfExecutionGraphicsItem(indexInsertedInto, unitOfExecution);
    repositionUnitsOfExecution();
}
#if 0
QRectF DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::boundingRect() const
{
    return m_BoundingRect;
}
void DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->save();
    painter->setBrush(Qt::transparent);

    //draw class name and surrounding box
    painter->setPen(m_ClassBorderPen);
    QRectF resizeBoundingRectTo;
    painter->drawText(boundingRect(), (Qt::AlignVCenter /*| Qt::TextDontClip*/), m_DesignEqualsImplementationClassLifeLine->designEqualsImplementationClass()->ClassName, &resizeBoundingRectTo); //TODOreq: unsafe pointer usage :)
    QRectF roundedRect = resizeBoundingRectTo;
    roundedRect.setLeft(roundedRect.left()-(DESIGNEQUALSIMPLEMENTATION_GUI_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT));
    roundedRect.setTop(roundedRect.top()-(DESIGNEQUALSIMPLEMENTATION_GUI_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT));
    roundedRect.setWidth(roundedRect.width()+(DESIGNEQUALSIMPLEMENTATION_GUI_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT));
    roundedRect.setHeight(roundedRect.height()+(DESIGNEQUALSIMPLEMENTATION_GUI_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT));
    painter->drawRoundedRect(roundedRect, DESIGNEQUALSIMPLEMENTATION_GUI_CLASS_GRAPHICS_ITEM_ROUNDED_RECTANGLE_RADIUS, DESIGNEQUALSIMPLEMENTATION_GUI_CLASS_GRAPHICS_ITEM_ROUNDED_RECTANGLE_RADIUS);

#if 0
    //draw [first] no activity life line
    painter->setPen(m_LifelineNoActivityPen);

    painter->drawLine(topLifeLine, bottomOfLifeLine);
#endif

    //(TODOreq: a lifeline with no use should be able to be auto-trimmed from all use cases in project (like inkscape's "vacuum defs"))

#if 0
    //draw [first] unit of execution rectangle

    //painter->drawRect()
    painter->drawRect(unitOfExecutionRect);
#endif

    //bleh I think I need aggregate items in order to [easily ('intersects')] see which unit of execution the arrow being drawn comes from... and i probably need to fix my bounding rect stupidity to support that

    m_BoundingRect = resizeBoundingRectTo;
    painter->restore();
}
#endif
