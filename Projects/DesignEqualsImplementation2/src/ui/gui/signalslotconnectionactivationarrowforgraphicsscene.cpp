#include "signalslotconnectionactivationarrowforgraphicsscene.h"

#include <QPen>

//TODOoptional: when doing a cross thread call, if there is color differentiation between the lifelines, it would be nifty to see the arrow transform colors halfway...
//TODOreq: this is just a line, but an arrow has some more graphics shit at p2, which should be a child of this
//TODOreq: the [signal and] slot invocation text should be rotated along with the angle, but actually i think i want all my lines to be exactly horizontal so nvm
SignalSlotConnectionActivationArrowForGraphicsScene::SignalSlotConnectionActivationArrowForGraphicsScene(QGraphicsItem *sourceGraphicsItem, int statementInsertIndex, const QLineF &line, int optionalSubInsertIndexForWhenConnectingSlotToExistingSignal_OrNegativeOneIfNotApplicable, QGraphicsItem *parent)
    : QGraphicsLineItem(line, parent)
    , m_SourceGraphicsItem(sourceGraphicsItem)
    , m_StatementInsertIndex(statementInsertIndex)
    , m_OptionalSubInsertIndexForWhenConnectingSlotToExistingSignal_OrNegativeOneIfNotApplicable(optionalSubInsertIndexForWhenConnectingSlotToExistingSignal_OrNegativeOneIfNotApplicable)
{
    QPen myPen;
    myPen.setWidth(2);
    setPen(myPen);
}
QGraphicsItem *SignalSlotConnectionActivationArrowForGraphicsScene::sourceGraphicsItem() const
{
    return m_SourceGraphicsItem;
}
int SignalSlotConnectionActivationArrowForGraphicsScene::statementInsertIndex() const //primarily for the statement index, but doubles as the "slot index" when the source is an existing signal
{
    return m_StatementInsertIndex;
}
int SignalSlotConnectionActivationArrowForGraphicsScene::optionalSubInsertIndexForWhenConnectingSlotToExistingSignal_OrNegativeOneIfNotApplicable() const
{
    return m_OptionalSubInsertIndexForWhenConnectingSlotToExistingSignal_OrNegativeOneIfNotApplicable;
}
