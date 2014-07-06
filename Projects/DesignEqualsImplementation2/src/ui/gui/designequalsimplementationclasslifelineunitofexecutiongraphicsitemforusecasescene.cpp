#include "designequalsimplementationclasslifelineunitofexecutiongraphicsitemforusecasescene.h"

#include "../../designequalsimplementationclasslifelineunitofexecution.h"

DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution, QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
    , m_UnitOfExecution(unitOfExecution)
{ }
DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution, const QRectF &rect, QGraphicsItem *parent)
    : QGraphicsRectItem(rect, parent)
    , m_UnitOfExecution(unitOfExecution)
{ }
DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent)
    : QGraphicsRectItem(x, y, w, h, parent)
    , m_UnitOfExecution(unitOfExecution)
{ }
