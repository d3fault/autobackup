#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEGRAPHICSITEMFORUSECASESCENE_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEGRAPHICSITEMFORUSECASESCENE_H

#include <QObject>
#include <QGraphicsRectItem>

#include <QPen>

class DesignEqualsImplementationClassLifeLine;
class DesignEqualsImplementationClassLifeLineUnitOfExecution;
class DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene;

class DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLine *classLifeLine, QObject *qobjectParent = 0, QGraphicsItem *graphicsItemParent = 0);
    explicit DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLine *classLifeLine, const QRectF &rect, QObject *qobjectParent = 0, QGraphicsItem *graphicsItemParent = 0);
    explicit DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLine *classLifeLine, qreal x, qreal y, qreal w, qreal h, QObject *qobjectParent = 0, QGraphicsItem *graphicsItemParent = 0);

    DesignEqualsImplementationClassLifeLine *classLifeLine() const;
    //virtual QRectF boundingRect() const;
    //virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    virtual int type() const;
private:
    QList<DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene*> m_DesignedOrderedButOnlySemiFlowOrderedUnitsOfExecution;
    QList<QGraphicsLineItem*> m_LinesJustAboveEachUnitOfExecution;
    DesignEqualsImplementationClassLifeLine *m_DesignEqualsImplementationClassLifeLine;
    QRectF m_BoundingRect;
    QPen m_ClassBorderPen;

    void privateConstructor(DesignEqualsImplementationClassLifeLine *classLifeLine);
    void insertUnitOfExecutionGraphicsItem(int indexInsertedInto, DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution);
    void repositionUnitsOfExecution();
private slots:
    void handleUnitOfExecutionInserted(int indexInsertedInto, DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEGRAPHICSITEMFORUSECASESCENE_H
