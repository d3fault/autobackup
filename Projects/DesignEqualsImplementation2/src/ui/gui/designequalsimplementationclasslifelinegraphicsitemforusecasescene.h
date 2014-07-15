#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEGRAPHICSITEMFORUSECASESCENE_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEGRAPHICSITEMFORUSECASESCENE_H

#include <QObject>
#include <QGraphicsRectItem>

#include <QPen>

class DesignEqualsImplementationClassLifeLine;
class DesignEqualsImplementationClassSlot;
//class DesignEqualsImplementationClassLifeLineUnitOfExecution;
class DesignEqualsImplementationSlotGraphicsItemForUseCaseScene;

class DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLine *classLifeLine, QObject *qobjectParent = 0, QGraphicsItem *graphicsItemParent = 0);
    explicit DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLine *classLifeLine, const QRectF &rect, QObject *qobjectParent = 0, QGraphicsItem *graphicsItemParent = 0);
    explicit DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLine *classLifeLine, qreal x, qreal y, qreal w, qreal h, QObject *qobjectParent = 0, QGraphicsItem *graphicsItemParent = 0);

    void repositionSlotsBecauseOneSlotSizeChanged();

    DesignEqualsImplementationClassLifeLine *classLifeLine() const;
    //virtual QRectF boundingRect() const;
    //virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    virtual int type() const;
private:
    QList<DesignEqualsImplementationSlotGraphicsItemForUseCaseScene*> m_SlotsInThisUseCase;
    QList<QGraphicsLineItem*> m_DottedLinesJustAboveEachSlot;
    DesignEqualsImplementationClassLifeLine *m_DesignEqualsImplementationClassLifeLine;
    QRectF m_BoundingRect;
    QPen m_ClassBorderPen;

    void privateConstructor(DesignEqualsImplementationClassLifeLine *classLifeLine);
    void insertSlotGraphicsItem(int indexInsertedInto, DesignEqualsImplementationClassSlot *slot);
private slots:
    void handleSlotInserted(int indexInsertedInto, DesignEqualsImplementationClassSlot *slot);
    void handleSlotGeometryChanged();
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEGRAPHICSITEMFORUSECASESCENE_H
