#ifndef SNAPPINGINDICATIONVISUALREPRESENTATION_H
#define SNAPPINGINDICATIONVISUALREPRESENTATION_H

#include <QGraphicsItem>

class IRepresentSnapGraphicsItemAndProxyGraphicsItem
{
public:
    explicit IRepresentSnapGraphicsItemAndProxyGraphicsItem(QGraphicsItem *itemProxyingFor, int insertIndexForProxyItem, QGraphicsItem *visualRepresentationParent, int optionalSubInsertIndexForWhenConnectingSlotToExistingSignal_OrNegativeOneIfNotApplicable = -1);
    virtual ~IRepresentSnapGraphicsItemAndProxyGraphicsItem();

    QGraphicsItem *visualRepresentation();

    QGraphicsItem *itemProxyingFor() const;
    int insertIndexForProxyItem() const;
    int optionalSubInsertIndexForWhenConnectingSlotToExistingSignal_OrNegativeOneIfNotApplicable() const;
protected:
    QGraphicsItem *m_VisualRepresentationParent;
    QGraphicsItem *m_VisualRepresentation;
    virtual QGraphicsItem *myVisualRepresentation(QGraphicsItem *parent)=0;
private:
    QGraphicsItem *m_ItemProxyingFor;
    int m_InsertIndexForProxyItem;
    int m_OptionalSubInsertIndexForWhenConnectingSlotToExistingSignal_OrNegativeOneIfNotApplicable;
};

#endif // SNAPPINGINDICATIONVISUALREPRESENTATION_H
