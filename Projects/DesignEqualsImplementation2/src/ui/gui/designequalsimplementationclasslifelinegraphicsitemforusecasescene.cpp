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
#define DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_HALF_WIDTH 7
#define DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_MINIMUM_VERTICAL_SIZE 50

//TODOreq: if a slot invoked on another thread (to the right) invokes us (via signal or whatever) by drawing an arrow back to that same unit of execution, a new unit of execution is auto-created right then and there for it
DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLine *classLifeLine, QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
{
    privateConstructor(classLifeLine);
}
DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLine *classLifeLine, const QRectF &rect, QGraphicsItem *parent)
    : QGraphicsRectItem(rect, parent)
{
    privateConstructor(classLifeLine);
}
DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLine *classLifeLine, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent)
    : QGraphicsRectItem(x, y, w, h, parent)
{
    privateConstructor(classLifeLine);
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

    QString lifeLineTitleText;
    if(classLifeLine->myInstanceInClassThatHasMe_OrZeroIfTopLevelObject())
    {
        lifeLineTitleText = classLifeLine->myInstanceInClassThatHasMe_OrZeroIfTopLevelObject()->m_DesignEqualsImplementationClass->ClassName + "* " /*exception to the rule, I want the star to be close to the type and not the class that owns me*/ + classLifeLine->myInstanceInClassThatHasMe_OrZeroIfTopLevelObject()->m_DesignEqualsImplementationClassThatHasMe->ClassName + "::" + classLifeLine->myInstanceInClassThatHasMe_OrZeroIfTopLevelObject()->VariableName;
    }
    else
    {
        lifeLineTitleText = classLifeLine->designEqualsImplementationClass()->ClassName;
    }
    QGraphicsTextItem *classNameTextItem = new QGraphicsTextItem(lifeLineTitleText, this);
    setRect(classNameTextItem->boundingRect());

    QPointF topOfLifeLine, bottomOfLifeLine;
    topOfLifeLine.setX((rect().right()-rect().left())/2);
    bottomOfLifeLine.setX(topOfLifeLine.x());
    topOfLifeLine.setY(rect().bottom());
    bottomOfLifeLine.setY(topOfLifeLine.y()+DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_NO_ACTIVITY_LIFELINE_MINIMUM_VERTICAL_GAP);
    QLineF lineAboveUnitOfExecution(topOfLifeLine, bottomOfLifeLine);
    Q_FOREACH(DesignEqualsImplementationClassLifeLineUnitOfExecution *currentUnitOfExecution, classLifeLine->unitsOfExecution())
    {
        QGraphicsLineItem *lineAboveUnitOfExecutionGraphicsItem = new QGraphicsLineItem(lineAboveUnitOfExecution, this);
        QPointF topLeftOfUnitOfExecutionRect(bottomOfLifeLine.x()-DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_HALF_WIDTH, bottomOfLifeLine.y());
        QPointF bottomRightOfUnitOfExecutionRect(bottomOfLifeLine.x()+DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_HALF_WIDTH, bottomOfLifeLine.y()+DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_MINIMUM_VERTICAL_SIZE);
        QRectF unitOfExecutionRect(topLeftOfUnitOfExecutionRect, bottomRightOfUnitOfExecutionRect);
        DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene *unitOfExecutionGraphicsItem = new DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene(currentUnitOfExecution, unitOfExecutionRect, this);

        //get next line in place, even if there aren't any more units of execution
        topOfLifeLine.setY(unitOfExecutionRect.bottom());
        bottomOfLifeLine.setY(topOfLifeLine.y()+DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_NO_ACTIVITY_LIFELINE_MINIMUM_VERTICAL_GAP);
        lineAboveUnitOfExecution.setPoints(topOfLifeLine, bottomOfLifeLine);
    }
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
