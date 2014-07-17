#include "snappingindicationvisualrepresentation.h"

ISnappingIndicationVisualRepresentation::ISnappingIndicationVisualRepresentation(QGraphicsItem *itemProxyingFor, int insertIndex, QGraphicsItem *visualRepresentationParent)
    : m_VisualRepresentationParent(visualRepresentationParent)
    , m_VisualRepresentation(0)
    , m_ItemProxyingFor(itemProxyingFor)
    , m_InsertIndex(insertIndex)
{ }
ISnappingIndicationVisualRepresentation::~ISnappingIndicationVisualRepresentation()
{
    delete m_VisualRepresentation;
}
QGraphicsItem *ISnappingIndicationVisualRepresentation::visualRepresentation()
{
    if(!m_VisualRepresentation)
        m_VisualRepresentation = myVisualRepresentation(m_VisualRepresentationParent);
    return m_VisualRepresentation;
}
QGraphicsItem *ISnappingIndicationVisualRepresentation::itemProxyingFor() const
{
    return m_ItemProxyingFor;
}
int ISnappingIndicationVisualRepresentation::insertIndexForProxyItem() const
{
    return m_InsertIndex;
}
