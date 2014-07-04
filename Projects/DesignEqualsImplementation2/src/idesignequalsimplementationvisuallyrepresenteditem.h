#ifndef IDESIGNEQUALSIMPLEMENTATIONVISUALLYREPRESENTEDITEM_H
#define IDESIGNEQUALSIMPLEMENTATIONVISUALLYREPRESENTEDITEM_H

#include <QPoint>

class IDesignEqualsImplementationVisuallyRepresentedItem
{
public:
    explicit IDesignEqualsImplementationVisuallyRepresentedItem() { }
    virtual ~IDesignEqualsImplementationVisuallyRepresentedItem() { }
    QPointF Position;
};

#endif // IDESIGNEQUALSIMPLEMENTATIONVISUALLYREPRESENTEDITEM_H
