#ifndef DESIGNEQUALSIMPLEMENTATIONEXISTINSIGNALGRAPHICSITEMFORUSECASESCENE_H
#define DESIGNEQUALSIMPLEMENTATIONEXISTINSIGNALGRAPHICSITEMFORUSECASESCENE_H

#include <QGraphicsLineItem>
#include "isnappablesourcegraphicsitem.h"

class QGraphicsRectItem;

class DesignEqualsImplementationClassSlot;
class DesignEqualsImplementationClassSignal;
class DesignEqualsImplementationClassLifeLine;
class UseCaseGraphicsScene;

class DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene : public QGraphicsLineItem, public ISnappableSourceGraphicsItem
{
public:
    explicit DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene(UseCaseGraphicsScene *parentUseCaseGraphicsScene, DesignEqualsImplementationClassLifeLine *sourceClassLifeline, DesignEqualsImplementationClassSlot *slotThatSignalWasEmittedFrom, int indexStatementInsertedInto, DesignEqualsImplementationClassSignal *theSignal, QGraphicsItem *parent = 0);
    virtual ~DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene() { }
    virtual int type() const;
    virtual IRepresentSnapGraphicsItemAndProxyGraphicsItem *makeSnappingHelperForMousePoint(QPointF eventScenePos);
private:
    DesignEqualsImplementationClassSignal *m_TheSignal;
    QGraphicsRectItem *m_NotchMultiplexerRect_OrZeroIfNoSlotsAttachedToSignal;
    QList<qreal> m_VerticalPositionsOfSnapPoints;
};

#endif // DESIGNEQUALSIMPLEMENTATIONEXISTINSIGNALGRAPHICSITEMFORUSECASESCENE_H
