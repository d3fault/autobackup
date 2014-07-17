#ifndef SNAPPINGINDICATIONVISUALREPRESENTATION_H
#define SNAPPINGINDICATIONVISUALREPRESENTATION_H

#include <QGraphicsItem>

class ISnappingIndicationVisualRepresentation
{
public:
    explicit ISnappingIndicationVisualRepresentation(QGraphicsItem *itemProxyingFor, int insertIndexForProxyItem, QGraphicsItem *visualRepresentationParent = 0);
    virtual ~ISnappingIndicationVisualRepresentation();

    QGraphicsItem *visualRepresentation();

    QGraphicsItem *itemProxyingFor() const;
    int insertIndexForProxyItem() const;
protected:
    QGraphicsItem *m_VisualRepresentationParent;
    QGraphicsItem *m_VisualRepresentation;
    virtual QGraphicsItem *myVisualRepresentation(QGraphicsItem *parent)=0;
private:
    QGraphicsItem *m_ItemProxyingFor;
    int m_InsertIndex;
};

#endif // SNAPPINGINDICATIONVISUALREPRESENTATION_H
