#include "sourceslotsnappingindicationvisualrepresentation.h"

#include <QGraphicsLineItem>
#include <QPen>

#include "designequalsimplementationguicommon.h"

SourceSlotSnappingIndicationVisualRepresentation::SourceSlotSnappingIndicationVisualRepresentation(QGraphicsItem *itemProxyingFor, int insertIndex, QGraphicsItem *parentGraphicsItem)
    : IRepresentSnapGraphicsItemAndProxyGraphicsItem(itemProxyingFor, insertIndex, parentGraphicsItem)
{
    visualRepresentation();
}
SourceSlotSnappingIndicationVisualRepresentation::~SourceSlotSnappingIndicationVisualRepresentation()
{ }
QGraphicsItem *SourceSlotSnappingIndicationVisualRepresentation::myVisualRepresentation(QGraphicsItem *parent)
{
    QGraphicsLineItem *lineGraphicItem = new QGraphicsLineItem(parent);
    lineGraphicItem->setLine(-5, 0, 5, 0);
    QPen myPen(DESIGNEQUALSIMPLEMENTATION_GUI_SNAP_INDICATION_COLOR);
    myPen.setWidth(2);
    lineGraphicItem->setPen(myPen);
    return lineGraphicItem;
}
