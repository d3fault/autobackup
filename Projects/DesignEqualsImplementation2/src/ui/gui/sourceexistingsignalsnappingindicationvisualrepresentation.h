#ifndef SOURCEEXISTINGSIGNALSNAPPINGINDICATIONVISUALREPRESENTATION_H
#define SOURCEEXISTINGSIGNALSNAPPINGINDICATIONVISUALREPRESENTATION_H

#include "snappingindicationvisualrepresentation.h"

class SourceExistingSignalSnappingIndicationVisualRepresentation : public IRepresentSnapGraphicsItemAndProxyGraphicsItem
{
public:
    explicit SourceExistingSignalSnappingIndicationVisualRepresentation(QGraphicsItem *itemProxyingFor, int statementIndexOfSignal, QGraphicsItem *parentGraphicsItem, int slotIndexIntoExistingSignal);
    virtual ~SourceExistingSignalSnappingIndicationVisualRepresentation();
protected:
    virtual QGraphicsItem *myVisualRepresentation(QGraphicsItem *parent);
};

#endif // SOURCEEXISTINGSIGNALSNAPPINGINDICATIONVISUALREPRESENTATION_H
