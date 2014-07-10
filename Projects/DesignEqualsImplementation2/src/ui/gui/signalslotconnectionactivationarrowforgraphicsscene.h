#ifndef SIGNALSLOTCONNECTIONACTIVATIONARROWFORGRAPHICSSCENE_H
#define SIGNALSLOTCONNECTIONACTIVATIONARROWFORGRAPHICSSCENE_H

#include <QGraphicsLineItem>

class SignalSlotConnectionActivationArrowForGraphicsScene : public QGraphicsLineItem
{
public:
    explicit SignalSlotConnectionActivationArrowForGraphicsScene(QGraphicsItem *sourceGraphicsItem, QGraphicsItem *parent = 0);
    explicit SignalSlotConnectionActivationArrowForGraphicsScene(QGraphicsItem *sourceGraphicsItem, const QLineF &line, QGraphicsItem *parent = 0);
    explicit SignalSlotConnectionActivationArrowForGraphicsScene(QGraphicsItem *sourceGraphicsItem, qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem *parent = 0);

    QGraphicsItem *sourceGraphicsItem() const;
private:
    QGraphicsItem *m_SourceGraphicsItem;

    void myConstructor(QGraphicsItem *sourceGraphicsItem);
};

#endif // SIGNALSLOTCONNECTIONACTIVATIONARROWFORGRAPHICSSCENE_H
