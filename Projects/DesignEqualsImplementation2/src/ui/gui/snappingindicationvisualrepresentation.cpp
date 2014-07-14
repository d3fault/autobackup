#include "snappingindicationvisualrepresentation.h"

#include <QPen>

SnappingIndicationVisualRepresentation::SnappingIndicationVisualRepresentation(QGraphicsItem *itemProxyingFor, int insertIndex, QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
    , m_ItemProxyingFor(itemProxyingFor)
    , m_InsertIndex(insertIndex)
{
    setLine(-5, 0, 5, 0);
    QPen myPen(Qt::red);
    myPen.setWidth(2);
    setPen(myPen);
}
QGraphicsItem *SnappingIndicationVisualRepresentation::itemProxyingFor() const
{
    return m_ItemProxyingFor;
}
int SnappingIndicationVisualRepresentation::insertIndex() const
{
    return m_InsertIndex;
}
