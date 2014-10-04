#include "designequalsimplementationexistinsignalgraphicsitemforusecasescene.h"

#include "designequalsimplementationguicommon.h"

DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene::DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    //TODOreq: draw differently depending on whether or not there is one slot connected, and of course account for multiple slots
}
int DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene::type() const
{
    return DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ExistingSignal_GRAPHICS_TYPE_ID;
}
