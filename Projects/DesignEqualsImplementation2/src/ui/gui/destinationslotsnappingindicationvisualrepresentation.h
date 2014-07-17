#ifndef DESTINATIONSLOTSNAPPINGINDICATIONVISUALREPRESENTATION_H
#define DESTINATIONSLOTSNAPPINGINDICATIONVISUALREPRESENTATION_H

#include "snappingindicationvisualrepresentation.h"

class DestinationSlotSnappingIndicationVisualRepresentation : public ISnappingIndicationVisualRepresentation, public QGraphicsLineItem
{
public:
    explicit DestinationSlotSnappingIndicationVisualRepresentation(QGraphicsItem *itemProxyingFor, int insertIndexForProxyItem, QGraphicsItem *parentGraphicsItem = 0);
    virtual ~DestinationSlotSnappingIndicationVisualRepresentation();
protected:
    virtual QGraphicsItem *myVisualRepresentation(QGraphicsItem *parent);
};

#endif // DESTINATIONSLOTSNAPPINGINDICATIONVISUALREPRESENTATION_H
