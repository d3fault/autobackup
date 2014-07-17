#ifndef SOURCESLOTSNAPPINGINDICATIONVISUALREPRESENTATION_H
#define SOURCESLOTSNAPPINGINDICATIONVISUALREPRESENTATION_H

#include "snappingindicationvisualrepresentation.h"

class QGraphicsItem;

class SourceSlotSnappingIndicationVisualRepresentation : public IRepresentSnapGraphicsItemAndProxyGraphicsItem
{
public:
    explicit SourceSlotSnappingIndicationVisualRepresentation(QGraphicsItem *itemProxyingFor, int insertIndexForProxyItem, QGraphicsItem *parentGraphicsItem);
    virtual ~SourceSlotSnappingIndicationVisualRepresentation();
protected:
    virtual QGraphicsItem *myVisualRepresentation(QGraphicsItem *parent);
};

#endif // SOURCESLOTSNAPPINGINDICATIONVISUALREPRESENTATION_H
