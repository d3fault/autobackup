#ifndef SIGNALSTATEMENTNOTCHMULTIPLEXTERGRAPHICSRECT_H
#define SIGNALSTATEMENTNOTCHMULTIPLEXTERGRAPHICSRECT_H

#include <QGraphicsRectItem>

class DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene;

class SignalStatementNotchMultiplexterGraphicsRect : public QGraphicsRectItem
{
public:
    explicit SignalStatementNotchMultiplexterGraphicsRect(DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene *parentSignalStatementGraphicsItem, const QRectF &rect, QGraphicsItem *parent);
    DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene *parentSignalStatementGraphicsItem() const;
    virtual int type() const;
private:
    DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene *m_ParentSignalStatementGraphicsItem;
};

#endif // SIGNALSTATEMENTNOTCHMULTIPLEXTERGRAPHICSRECT_H
