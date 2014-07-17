#ifndef SNAPPINGINDICATIONVISUALREPRESENTATION_H
#define SNAPPINGINDICATIONVISUALREPRESENTATION_H

#include <QGraphicsItem>

class IRepresentSnapGraphicsItemAndProxyGraphicsItem
{
public:
    explicit IRepresentSnapGraphicsItemAndProxyGraphicsItem(QGraphicsItem *itemProxyingFor, int insertIndexForProxyItem, QGraphicsItem *visualRepresentationParent = 0);
    virtual ~IRepresentSnapGraphicsItemAndProxyGraphicsItem();

    QGraphicsItem *visualRepresentation();

    QGraphicsItem *itemProxyingFor() const;
    int insertIndexForProxyItem() const;
protected:
    QGraphicsItem *m_VisualRepresentationParent;
    QGraphicsItem *m_VisualRepresentation;
    virtual QGraphicsItem *myVisualRepresentation(QGraphicsItem *parent)=0;
private:
    QGraphicsItem *m_ItemProxyingFor;
    int m_InsertIndexForProxyItem;
};

#endif // SNAPPINGINDICATIONVISUALREPRESENTATION_H
