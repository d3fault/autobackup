#ifndef DESIGNEQUALSIMPLEMENTATIONSLOTINVOKEGRAPHICSITEMFORUSECASESCENE_H
#define DESIGNEQUALSIMPLEMENTATIONSLOTINVOKEGRAPHICSITEMFORUSECASESCENE_H

#include <QGraphicsLineItem>

class DesignEqualsImplementationSlotGraphicsItemForUseCaseScene;
class DesignEqualsImplementationSlotInvocationStatement;

class DesignEqualsImplementationSlotInvokeGraphicsItemForUseCaseScene : public QGraphicsLineItem
{
public:
    DesignEqualsImplementationSlotInvokeGraphicsItemForUseCaseScene(DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *parentSourceSlotGraphicsItem, DesignEqualsImplementationSlotInvocationStatement *slotInvokeStatement, QGraphicsItem *parent = 0);
private:
    DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *m_ParentSourceSlotGraphicsItem;
};

#endif // DESIGNEQUALSIMPLEMENTATIONSLOTINVOKEGRAPHICSITEMFORUSECASESCENE_H
