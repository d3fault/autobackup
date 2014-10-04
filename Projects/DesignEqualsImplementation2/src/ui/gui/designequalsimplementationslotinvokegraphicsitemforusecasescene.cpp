#include "designequalsimplementationslotinvokegraphicsitemforusecasescene.h"

DesignEqualsImplementationSlotInvokeGraphicsItemForUseCaseScene::DesignEqualsImplementationSlotInvokeGraphicsItemForUseCaseScene(QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
{
    setLine(QLineF(QPointF(-5, 0), QPointF(5, 0)));
}
