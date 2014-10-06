#include "sourceexistingsignalsnappingindicationvisualrepresentation.h"

#include <QPen>

#include "designequalsimplementationguicommon.h"

SourceExistingSignalSnappingIndicationVisualRepresentation::SourceExistingSignalSnappingIndicationVisualRepresentation(QGraphicsItem *itemProxyingFor, int statementIndexOfSignal, QGraphicsItem *parentGraphicsItem, int slotIndexIntoExistingSignal)
    : IRepresentSnapGraphicsItemAndProxyGraphicsItem(itemProxyingFor, statementIndexOfSignal, parentGraphicsItem, slotIndexIntoExistingSignal)
{
    visualRepresentation();
}
SourceExistingSignalSnappingIndicationVisualRepresentation::~SourceExistingSignalSnappingIndicationVisualRepresentation()
{ }
QGraphicsItem *SourceExistingSignalSnappingIndicationVisualRepresentation::myVisualRepresentation(QGraphicsItem *parent)
{
    QGraphicsLineItem *lineGraphicItem = new QGraphicsLineItem(parent);
    lineGraphicItem->setLine(0, 0, 10, 0);
    QPen myPen(DESIGNEQUALSIMPLEMENTATION_GUI_SNAP_INDICATION_COLOR);
    myPen.setWidth(2);
    lineGraphicItem->setPen(myPen);
    return lineGraphicItem;
}
