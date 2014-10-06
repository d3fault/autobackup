#ifndef DESIGNEQUALSIMPLEMENTATIONEXISTINSIGNALGRAPHICSITEMFORUSECASESCENE_H
#define DESIGNEQUALSIMPLEMENTATIONEXISTINSIGNALGRAPHICSITEMFORUSECASESCENE_H

#include <QGraphicsLineItem>
#include "isnappablesourcegraphicsitem.h"

class QGraphicsRectItem;

class DesignEqualsImplementationClassSlot;
class DesignEqualsImplementationClassSignal;
class DesignEqualsImplementationClassLifeLine;
class UseCaseGraphicsScene;
class SignalStatementNotchMultiplexterGraphicsRect;

#define DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS 2.5

class DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene : public QGraphicsLineItem, public ISnappableSourceGraphicsItem
{
public:
    explicit DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene(UseCaseGraphicsScene *parentUseCaseGraphicsScene, DesignEqualsImplementationClassLifeLine *sourceClassLifeline, DesignEqualsImplementationClassSlot *slotThatSignalWasEmittedFrom, int indexStatementInsertedInto, DesignEqualsImplementationClassSignal *underlyingSignal, QGraphicsItem *parent = 0);
    virtual ~DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene() { }
    virtual int type() const;
    virtual IRepresentSnapGraphicsItemAndProxyGraphicsItem *makeSnappingHelperForMousePoint(QPointF eventScenePos);
    DesignEqualsImplementationClassSignal *underlyingSignal() const;
    QList<qreal> verticalPositionsOfSnapPoints() const;
    DesignEqualsImplementationClassSlot *slotThatSignalWasEmittedFrom() const;
    DesignEqualsImplementationClassLifeLine *sourceClassLifeline() const;
    int indexStatementInsertedInto() const;
private:
    DesignEqualsImplementationClassSignal *m_UnderlyingSignal;
    SignalStatementNotchMultiplexterGraphicsRect *m_NotchMultiplexerRect_OrZeroIfNoSlotsAttachedToSignal;
    QList<qreal> m_VerticalPositionsOfSnapPoints;

    DesignEqualsImplementationClassLifeLine *m_SourceClassLifeline;
    DesignEqualsImplementationClassSlot *m_SlotThatSignalWasEmittedFrom;

    int m_IndexStatementInsertedInto;
};

#endif // DESIGNEQUALSIMPLEMENTATIONEXISTINSIGNALGRAPHICSITEMFORUSECASESCENE_H
