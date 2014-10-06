#ifndef SIGNALSTATEMENTNOTCHMULTIPLEXTERGRAPHICSRECT_H
#define SIGNALSTATEMENTNOTCHMULTIPLEXTERGRAPHICSRECT_H

#include <QGraphicsRectItem>
#include "isnappablesourcegraphicsitem.h"

class DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene;

class SignalStatementNotchMultiplexterGraphicsRect : public QGraphicsRectItem, public ISnappableSourceGraphicsItem
{
public:
    explicit SignalStatementNotchMultiplexterGraphicsRect(DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene *parentSignalStatementGraphicsItem, const QRectF &rect, QGraphicsItem *parent);
    DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene *parentSignalStatementGraphicsItem() const;
    virtual int type() const;
    virtual IRepresentSnapGraphicsItemAndProxyGraphicsItem *makeSnappingHelperForMousePoint(QPointF eventScenePos);
    virtual ~SignalStatementNotchMultiplexterGraphicsRect() { }
private:
    DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene *m_ParentSignalStatementGraphicsItem;
};

#endif // SIGNALSTATEMENTNOTCHMULTIPLEXTERGRAPHICSRECT_H
