#include "sourceslotsnappingindicationvisualrepresentation.h"

#include <QGraphicsLineItem>
#include <QPen>

SourceSlotSnappingIndicationVisualRepresentation::SourceSlotSnappingIndicationVisualRepresentation(QGraphicsItem *itemProxyingFor, int insertIndex, QGraphicsItem *parentGraphicsItem)
    : ISnappingIndicationVisualRepresentation(itemProxyingFor, insertIndex, parentGraphicsItem)
{
    m_VisualRepresentation = myVisualRepresentation(parentGraphicsItem);
}
SourceSlotSnappingIndicationVisualRepresentation::~SourceSlotSnappingIndicationVisualRepresentation()
{ }
QGraphicsItem *SourceSlotSnappingIndicationVisualRepresentation::myVisualRepresentation(QGraphicsItem *parent)
{
    QGraphicsLineItem *lineGraphicItem = new QGraphicsLineItem(parent);
    lineGraphicItem->setLine(-5, 0, 5, 0);
    QPen myPen(Qt::red);
    myPen.setWidth(2);
    lineGraphicItem->setPen(myPen);
    return lineGraphicItem;
}
