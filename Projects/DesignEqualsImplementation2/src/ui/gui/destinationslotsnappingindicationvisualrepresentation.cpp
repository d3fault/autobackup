#include "destinationslotsnappingindicationvisualrepresentation.h"

#include <QGraphicsEllipseItem>
#include <QPen>
#include <QRectF>

#include "designequalsimplementationguicommon.h"

#define DestinationSlotSnappingIndicationVisualRepresentation_ELLIPSE_WIDTH 15

DestinationSlotSnappingIndicationVisualRepresentation::DestinationSlotSnappingIndicationVisualRepresentation(QGraphicsItem *itemProxyingFor, int insertIndex, QGraphicsItem *parentGraphicsItem)
    : IRepresentSnapGraphicsItemAndProxyGraphicsItem(itemProxyingFor, insertIndex, parentGraphicsItem)
{
    visualRepresentation();
}
DestinationSlotSnappingIndicationVisualRepresentation::~DestinationSlotSnappingIndicationVisualRepresentation()
{ }
QGraphicsItem *DestinationSlotSnappingIndicationVisualRepresentation::myVisualRepresentation(QGraphicsItem *parent)
{
    QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem(parent);
    QPointF myTopLeft(-(DestinationSlotSnappingIndicationVisualRepresentation_ELLIPSE_WIDTH/2), -(DestinationSlotSnappingIndicationVisualRepresentation_ELLIPSE_WIDTH/2));
    QPointF myBottomRight((DestinationSlotSnappingIndicationVisualRepresentation_ELLIPSE_WIDTH/2), (DestinationSlotSnappingIndicationVisualRepresentation_ELLIPSE_WIDTH/2));
    QRectF myRect(myTopLeft, myBottomRight);
    ellipse->setRect(myRect);
    QPen myPen(DESIGNEQUALSIMPLEMENTATION_GUI_SNAP_INDICATION_COLOR);
    myPen.setWidth(2);
    ellipse->setPen(myPen);
    return ellipse;
}
