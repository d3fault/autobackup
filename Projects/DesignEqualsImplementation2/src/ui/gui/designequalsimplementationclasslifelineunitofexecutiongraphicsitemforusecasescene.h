#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEUNITOFEXECUTIONGRAPHICSITEMFORUSECASESCENE_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEUNITOFEXECUTIONGRAPHICSITEMFORUSECASESCENE_H

#include <QGraphicsRectItem>

class DesignEqualsImplementationClassLifeLineUnitOfExecution;

class DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene : public QGraphicsRectItem
{
public:
    explicit DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution, QGraphicsItem *parent = 0);
    explicit DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution, const QRectF &rect, QGraphicsItem *parent = 0);
    explicit DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = 0);
    DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution() const;
    virtual int type() const;
private:
    DesignEqualsImplementationClassLifeLineUnitOfExecution *m_UnitOfExecution;
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEUNITOFEXECUTIONGRAPHICSITEMFORUSECASESCENE_H
