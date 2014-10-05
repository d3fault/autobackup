#include "signalstatementnotchmultiplextergraphicsrect.h"

#include "designequalsimplementationguicommon.h"

SignalStatementNotchMultiplexterGraphicsRect::SignalStatementNotchMultiplexterGraphicsRect(DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene *parentSignalStatementGraphicsItem, const QRectF &rect, QGraphicsItem *parent)
    : QGraphicsRectItem(rect, parent)
    , m_ParentSignalStatementGraphicsItem(parentSignalStatementGraphicsItem)
{ }
DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene *SignalStatementNotchMultiplexterGraphicsRect::parentSignalStatementGraphicsItem() const
{
    return m_ParentSignalStatementGraphicsItem;
}
int SignalStatementNotchMultiplexterGraphicsRect::type() const
{
    return DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ExistingSignal_SignalStatementNotchMultiplexterGraphicsRect_GRAPHICS_TYPE_ID;
}
