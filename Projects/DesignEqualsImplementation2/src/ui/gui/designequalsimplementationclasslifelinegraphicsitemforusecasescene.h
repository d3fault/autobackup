#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEGRAPHICSITEMFORUSECASESCENE_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEGRAPHICSITEMFORUSECASESCENE_H

#include <QObject>
#include <QGraphicsRectItem>

#include <QPen>

class QGraphicsSceneContextMenuEvent;

class UseCaseGraphicsScene;
class DesignEqualsImplementationType;
class DesignEqualsImplementationClass;
class DesignEqualsImplementationClassLifeLine;
class DesignEqualsImplementationClassSlot;
class DesignEqualsImplementationSlotGraphicsItemForUseCaseScene;

class DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(UseCaseGraphicsScene *parentUseCaseGraphicsScene, DesignEqualsImplementationClassLifeLine *classLifeLine, QObject *qobjectParent = 0, QGraphicsItem *graphicsItemParent = 0);

    void repositionSlotsBecauseOneSlotsChanged();

    DesignEqualsImplementationClassLifeLine *classLifeLine() const;
    //virtual QRectF boundingRect() const;
    //virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    virtual int type() const;
    UseCaseGraphicsScene *parentUseCaseGraphicsScene() const;

    void maybeMoveLeftOrRightBecauseNewSlotInvokeStatementWasConnected();

    DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *slotGraphicsItemForSlot_OrZeroIfNotFound(DesignEqualsImplementationClassSlot *theSlot);
protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
private:
    QList<DesignEqualsImplementationSlotGraphicsItemForUseCaseScene*> m_SlotGraphicsItemsInThisClassLifeLine;
    QList<QGraphicsLineItem*> m_DottedLinesJustAboveEachSlot;
    DesignEqualsImplementationClassLifeLine *m_DesignEqualsImplementationClassLifeLine;
    QRectF m_BoundingRect;
    QPen m_ClassBorderPen;

    UseCaseGraphicsScene *m_ParentUseCaseGraphicsScene;

    void privateConstructor(DesignEqualsImplementationClassLifeLine *classLifeLine);
    DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *createAndInsertSlotGraphicsItem(int indexInsertedInto, DesignEqualsImplementationClassSlot *slot);
signals:
    void slotGraphicsItemInsertedIntoClassLifeLineGraphicsItem(DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *slotGraphicsItem);
    void editCppModeRequested(DesignEqualsImplementationType *designEqualsImplementationClass);
private slots:
    void handleSlotInsertedIntoClassLifeLine(int indexInsertedInto, DesignEqualsImplementationClassSlot *slot);
    void handleSlotRemovedFromClassLifeLine(DesignEqualsImplementationClassSlot *slotRemoved);
    void handleSlotGeometryChanged();
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEGRAPHICSITEMFORUSECASESCENE_H
