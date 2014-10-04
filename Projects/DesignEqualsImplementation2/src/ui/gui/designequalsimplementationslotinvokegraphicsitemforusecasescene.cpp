#include "designequalsimplementationslotinvokegraphicsitemforusecasescene.h"

#include <QPen>

#include "designequalsimplementationguicommon.h"
#include "designequalsimplementationclasslifelinegraphicsitemforusecasescene.h"
#include "designequalsimplementationclasslifelineslotgraphicsitemforusecasescene.h"
#include "usecasegraphicsscene.h"
#include "../../designequalsimplementationclassslot.h"
#include "../../designequalsimplementationslotinvocationstatement.h"

//TODOoptional: ditto as with signal emit's "all statements share max width", in fact we might even want to compare them across statement TYPES (but at the time of writing, only slot invoke 'touches' ANOTHER class lifeline. signals and private methods do not. so maybe comparing across types is unwarranted)
DesignEqualsImplementationSlotInvokeGraphicsItemForUseCaseScene::DesignEqualsImplementationSlotInvokeGraphicsItemForUseCaseScene(DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *parentSourceSlotGraphicsItem, DesignEqualsImplementationSlotInvocationStatement *slotInvokeStatement, QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
    , m_ParentSourceSlotGraphicsItem(parentSourceSlotGraphicsItem)
{
    QPen myPen = pen();
    myPen.setWidth(3);
    myPen.setColor(Qt::red);
    setPen(myPen);

    QGraphicsTextItem *slotNameTextGraphicsItem = new QGraphicsTextItem(slotInvokeStatement->slotToInvoke()->Name, this);
    QRectF myChildrenBoundingRect = childrenBoundingRect();

    qreal startX = 0.0;
    if(parentItem()) //this seems hacky, but the parent calling setPos on us (a line) apparently doesn't work. i think as an exception to the rule, lines are "positioned" via their setLine
    {
        startX = parentItem()->boundingRect().width()/2; //TODOoptional: if the slot is being emitted to "the left" this would be negative
    }

    slotNameTextGraphicsItem->setPos(startX, -myChildrenBoundingRect.height());

    setLine(QLineF(QPointF(startX, 0), QPointF(myChildrenBoundingRect.width()+DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_LINE_WIDTH_MARGIN_AROUND_SIGNAL_NAME_TEXT, 0)));

    //TODOreq: move the destination class lifeline as far left as possible, so that it touches the slot invocation line we just drew
    DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *destinationClassLifelineMaybe = 0;
#if 0
    Q_FOREACH(DesignEqualsImplementationClassLifeLine *currentClassLifeline, slotInvokeStatement->classLifelineWhoseSlotIsToBeInvoked()->parentUseCase()->classLifeLines())
    {

    }
#endif
    //slotInvokeStatement->classLifelineWhoseSlotIsToBeInvoked()->maybeMoveLeftOrRightBecauseNewSlotInvokeStatementWasConnected();
    destinationClassLifelineMaybe = m_ParentSourceSlotGraphicsItem->parentClassLifelineGraphicsItem()->parentUseCaseGraphicsScene()->classLifelineGraphicsItemByClassLifeline_OrZeroIfNotFound(slotInvokeStatement->classLifelineWhoseSlotIsToBeInvoked()); //TODOreq: has to take into account sibling slot invocations (and possibly other statement types?)
    if(destinationClassLifelineMaybe)
    {
        destinationClassLifelineMaybe->maybeMoveLeftOrRightBecauseNewSlotInvokeStatementWasConnected();
    }
}
