#ifndef SIGNALSLOTCONNECTIONACTIVATIONARROWFORGRAPHICSSCENE_H
#define SIGNALSLOTCONNECTIONACTIVATIONARROWFORGRAPHICSSCENE_H

#include <QGraphicsLineItem>

class SignalSlotConnectionActivationArrowForGraphicsScene : public QGraphicsLineItem
{
public:
    explicit SignalSlotConnectionActivationArrowForGraphicsScene(QGraphicsItem *sourceGraphicsItem, int statementInsertIndex, const QLineF &line, int optionalSubInsertIndexForWhenConnectingSlotToExistingSignal_OrNegativeOneIfNotApplicable = -1, QGraphicsItem *parent = 0);

    QGraphicsItem *sourceGraphicsItem() const;
    int statementInsertIndex() const;
    int optionalSubInsertIndexForWhenConnectingSlotToExistingSignal_OrNegativeOneIfNotApplicable() const;
private:
    QGraphicsItem *m_SourceGraphicsItem;
    int m_StatementInsertIndex;
    int m_OptionalSubInsertIndexForWhenConnectingSlotToExistingSignal_OrNegativeOneIfNotApplicable;
};

#endif // SIGNALSLOTCONNECTIONACTIVATIONARROWFORGRAPHICSSCENE_H
