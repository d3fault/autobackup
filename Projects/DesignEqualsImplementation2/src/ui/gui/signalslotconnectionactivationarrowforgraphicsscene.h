#ifndef SIGNALSLOTCONNECTIONACTIVATIONARROWFORGRAPHICSSCENE_H
#define SIGNALSLOTCONNECTIONACTIVATIONARROWFORGRAPHICSSCENE_H

#include <QGraphicsLineItem>

class SignalSlotConnectionActivationArrowForGraphicsScene : public QGraphicsLineItem
{
public:
    explicit SignalSlotConnectionActivationArrowForGraphicsScene(QGraphicsItem *parent = 0);
    explicit SignalSlotConnectionActivationArrowForGraphicsScene(const QLineF &line, QGraphicsItem *parent = 0);
    explicit SignalSlotConnectionActivationArrowForGraphicsScene(qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem *parent = 0);
private:
    void myConstructor();
};

#endif // SIGNALSLOTCONNECTIONACTIVATIONARROWFORGRAPHICSSCENE_H
