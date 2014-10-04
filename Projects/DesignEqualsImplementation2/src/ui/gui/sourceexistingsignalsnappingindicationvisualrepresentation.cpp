#include "sourceexistingsignalsnappingindicationvisualrepresentation.h"

#include <QPen>

#include "designequalsimplementationguicommon.h"

SourceExistingSignalSnappingIndicationVisualRepresentation::SourceExistingSignalSnappingIndicationVisualRepresentation(QGraphicsItem *itemProxyingFor, int insertIndexForProxyItem, QGraphicsItem *parentGraphicsItem)
    : IRepresentSnapGraphicsItemAndProxyGraphicsItem(itemProxyingFor, insertIndexForProxyItem, parentGraphicsItem)
{
    visualRepresentation();
}
SourceExistingSignalSnappingIndicationVisualRepresentation::~SourceExistingSignalSnappingIndicationVisualRepresentation()
{ }
QGraphicsItem *SourceExistingSignalSnappingIndicationVisualRepresentation::myVisualRepresentation(QGraphicsItem *parent)
{
    QGraphicsLineItem *lineGraphicItem = new QGraphicsLineItem(parent);
    lineGraphicItem->setLine(-5, 0, 5, 0);
    QPen myPen(DESIGNEQUALSIMPLEMENTATION_GUI_SNAP_INDICATION_COLOR);
    myPen.setWidth(2);
    lineGraphicItem->setPen(myPen);
    return lineGraphicItem;
}
