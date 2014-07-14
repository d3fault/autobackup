#include "snappingindicationvisualrepresentation.h"

#include <QPen>

SnappingIndicationVisualRepresentation::SnappingIndicationVisualRepresentation(QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
{
    setLine(- 10, 0, 10, 0);
    setPen(QPen(Qt::red));
}
