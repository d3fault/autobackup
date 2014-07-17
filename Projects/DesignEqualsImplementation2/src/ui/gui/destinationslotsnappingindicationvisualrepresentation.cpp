#include "destinationslotsnappingindicationvisualrepresentation.h"

#include <QGraphicsEllipseItem>
#include <QPen>
#include <QRectF>

#define DestinationSlotSnappingIndicationVisualRepresentation_ELLIPSE_WIDTH 15

DestinationSlotSnappingIndicationVisualRepresentation::DestinationSlotSnappingIndicationVisualRepresentation(QGraphicsItem *itemProxyingFor, int insertIndex, QGraphicsItem *parentGraphicsItem)
    : IRepresentSnapGraphicsItemAndProxyGraphicsItem(itemProxyingFor, insertIndex, parentGraphicsItem)
{
    m_VisualRepresentation = myVisualRepresentation(parentGraphicsItem);
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
    QPen myPen(Qt::red);
    myPen.setWidth(2);
    ellipse->setPen(myPen);
    return ellipse;
}
