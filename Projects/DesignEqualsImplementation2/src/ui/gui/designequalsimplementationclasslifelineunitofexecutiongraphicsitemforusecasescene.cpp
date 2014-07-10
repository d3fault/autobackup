#include "designequalsimplementationclasslifelineunitofexecutiongraphicsitemforusecasescene.h"

#include "designequalsimplementationguicommon.h"
#include "../../designequalsimplementationclasslifelineunitofexecution.h"

DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution, DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifeline, QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
    , m_UnitOfExecution(unitOfExecution)
    , m_ParentClassLifeline(parentClassLifeline)
{ }
DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution, DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifeline, const QRectF &rect, QGraphicsItem *parent)
    : QGraphicsRectItem(rect, parent)
    , m_UnitOfExecution(unitOfExecution)
    , m_ParentClassLifeline(parentClassLifeline)
{ }
DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution, DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifeline, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent)
    : QGraphicsRectItem(x, y, w, h, parent)
    , m_UnitOfExecution(unitOfExecution)
    , m_ParentClassLifeline(parentClassLifeline)
{ }
DesignEqualsImplementationClassLifeLineUnitOfExecution *DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::unitOfExecution() const
{
    return m_UnitOfExecution;
}
DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::parentClassLifeline() const
{
    return m_ParentClassLifeline;
}
int DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene::type() const
{
    return DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassLifeLineUnitOfExecution_GRAPHICS_TYPE_ID;
}
