#include "snappingindicationvisualrepresentation.h"

IRepresentSnapGraphicsItemAndProxyGraphicsItem::IRepresentSnapGraphicsItemAndProxyGraphicsItem(QGraphicsItem *itemProxyingFor, int insertIndexForProxyItem, QGraphicsItem *visualRepresentationParent, int optionalSubInsertIndexForWhenConnectingSlotToExistingSignal_OrNegativeOneIfNotApplicable)
    : m_VisualRepresentationParent(visualRepresentationParent)
    , m_VisualRepresentation(0)
    , m_ItemProxyingFor(itemProxyingFor)
    , m_InsertIndexForProxyItem(insertIndexForProxyItem)
    , m_OptionalSubInsertIndexForWhenConnectingSlotToExistingSignal_OrNegativeOneIfNotApplicable(optionalSubInsertIndexForWhenConnectingSlotToExistingSignal_OrNegativeOneIfNotApplicable)
{ }
IRepresentSnapGraphicsItemAndProxyGraphicsItem::~IRepresentSnapGraphicsItemAndProxyGraphicsItem()
{
    if(m_VisualRepresentation)
        delete m_VisualRepresentation;
}
QGraphicsItem *IRepresentSnapGraphicsItemAndProxyGraphicsItem::visualRepresentation()
{
    if(!m_VisualRepresentation)
        m_VisualRepresentation = myVisualRepresentation(m_VisualRepresentationParent);
    return m_VisualRepresentation;
}
QGraphicsItem *IRepresentSnapGraphicsItemAndProxyGraphicsItem::itemProxyingFor() const
{
    return m_ItemProxyingFor;
}
int IRepresentSnapGraphicsItemAndProxyGraphicsItem::insertIndexForProxyItem() const
{
    return m_InsertIndexForProxyItem;
}
int IRepresentSnapGraphicsItemAndProxyGraphicsItem::optionalSubInsertIndexForWhenConnectingSlotToExistingSignal_OrNegativeOneIfNotApplicable() const
{
    return m_OptionalSubInsertIndexForWhenConnectingSlotToExistingSignal_OrNegativeOneIfNotApplicable;
}
