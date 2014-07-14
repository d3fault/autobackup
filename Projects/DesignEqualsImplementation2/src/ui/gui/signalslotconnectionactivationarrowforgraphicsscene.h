#ifndef SIGNALSLOTCONNECTIONACTIVATIONARROWFORGRAPHICSSCENE_H
#define SIGNALSLOTCONNECTIONACTIVATIONARROWFORGRAPHICSSCENE_H

#include <QGraphicsLineItem>

class SignalSlotConnectionActivationArrowForGraphicsScene : public QGraphicsLineItem
{
public:
    explicit SignalSlotConnectionActivationArrowForGraphicsScene(QGraphicsItem *sourceGraphicsItem, int statementInsertIndex, QGraphicsItem *parent = 0);
    explicit SignalSlotConnectionActivationArrowForGraphicsScene(QGraphicsItem *sourceGraphicsItem, int statementInsertIndex, const QLineF &line, QGraphicsItem *parent = 0);
    explicit SignalSlotConnectionActivationArrowForGraphicsScene(QGraphicsItem *sourceGraphicsItem, int statementInsertIndex, qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem *parent = 0);

    QGraphicsItem *sourceGraphicsItem() const;
    int statementInsertIndex() const;
private:
    QGraphicsItem *m_SourceGraphicsItem;
    int m_StatementInsertIndex;

    void myConstructor(QGraphicsItem *sourceGraphicsItem, int statementInsertIndex);
};

#endif // SIGNALSLOTCONNECTIONACTIVATIONARROWFORGRAPHICSSCENE_H
