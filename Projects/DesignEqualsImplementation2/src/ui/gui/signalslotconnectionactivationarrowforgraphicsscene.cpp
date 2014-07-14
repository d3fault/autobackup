#include "signalslotconnectionactivationarrowforgraphicsscene.h"

#include <QPen>

//TODOoptional: when doing a cross thread call, if there is color differentiation between the lifelines, it would be nifty to see the arrow transform colors halfway...
//TODOreq: this is just a line, but an arrow has some more graphics shit at p2, which should be a child of this
//TODOreq: the [signal and] slot invocation text should be rotated along with the angle, but actually i think i want all my lines to be exactly horizontal so nvm
SignalSlotConnectionActivationArrowForGraphicsScene::SignalSlotConnectionActivationArrowForGraphicsScene(QGraphicsItem *sourceGraphicsItem, int statementInsertIndex, QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
{
    myConstructor(sourceGraphicsItem, statementInsertIndex);
}
SignalSlotConnectionActivationArrowForGraphicsScene::SignalSlotConnectionActivationArrowForGraphicsScene(QGraphicsItem *sourceGraphicsItem, int statementInsertIndex, const QLineF &line, QGraphicsItem *parent)
    : QGraphicsLineItem(line, parent)
{
    myConstructor(sourceGraphicsItem, statementInsertIndex);
}
SignalSlotConnectionActivationArrowForGraphicsScene::SignalSlotConnectionActivationArrowForGraphicsScene(QGraphicsItem *sourceGraphicsItem, int statementInsertIndex, qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem *parent)
    : QGraphicsLineItem(x1, y1, x2, y2, parent)
{
    myConstructor(sourceGraphicsItem, statementInsertIndex);
}
QGraphicsItem *SignalSlotConnectionActivationArrowForGraphicsScene::sourceGraphicsItem() const
{
    return m_SourceGraphicsItem;
}
void SignalSlotConnectionActivationArrowForGraphicsScene::myConstructor(QGraphicsItem *sourceGraphicsItem, int statementInsertIndex)
{
    m_SourceGraphicsItem = sourceGraphicsItem;
    m_StatementInsertIndex = statementInsertIndex;

    QPen myPen;
    myPen.setWidth(2);
    setPen(myPen);
}
