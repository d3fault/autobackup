#include "designequalsimplementationprivatemethodinvokestatementgraphicsitemforusecasescene.h"

DesignEqualsImplementationPrivateMethodInvokeStatementGraphicsItemForUseCaseScene::DesignEqualsImplementationPrivateMethodInvokeStatementGraphicsItemForUseCaseScene(QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
{
    setLine(QLineF(QPointF(0,0), QPointF(10, 0)));
}
