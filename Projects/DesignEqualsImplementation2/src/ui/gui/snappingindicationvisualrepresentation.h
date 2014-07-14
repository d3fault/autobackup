#ifndef SNAPPINGINDICATIONVISUALREPRESENTATION_H
#define SNAPPINGINDICATIONVISUALREPRESENTATION_H

#include <QGraphicsLineItem>

class SnappingIndicationVisualRepresentation : public QGraphicsLineItem
{
public:
    explicit SnappingIndicationVisualRepresentation(QGraphicsItem *itemProxyingFor, int insertIndex, QGraphicsItem *parent = 0);

    QGraphicsItem *itemProxyingFor() const;
    int insertIndex() const;
private:
    QGraphicsItem *m_ItemProxyingFor;
    int m_InsertIndex;
};

#endif // SNAPPINGINDICATIONVISUALREPRESENTATION_H
