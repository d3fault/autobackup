#ifndef DESIGNEQUALSIMPLEMENTATIONEXISTINSIGNALGRAPHICSITEMFORUSECASESCENE_H
#define DESIGNEQUALSIMPLEMENTATIONEXISTINSIGNALGRAPHICSITEMFORUSECASESCENE_H

#include <QGraphicsLineItem>


class DesignEqualsImplementationClassSlot;
class DesignEqualsImplementationClassSignal;
class DesignEqualsImplementationClassLifeLine;
class UseCaseGraphicsScene;

class DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene : public QGraphicsLineItem
{
public:
    explicit DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene(UseCaseGraphicsScene *parentUseCaseGraphicsScene, DesignEqualsImplementationClassLifeLine *sourceClassLifeline, DesignEqualsImplementationClassSlot *slotThatSignalWasEmittedFrom, int indexStatementInsertedInto, DesignEqualsImplementationClassSignal *theSignal, QGraphicsItem *parent = 0);
    virtual ~DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene() { }
    virtual int type() const;
private:
    DesignEqualsImplementationClassSignal *m_TheSignal;
};

#endif // DESIGNEQUALSIMPLEMENTATIONEXISTINSIGNALGRAPHICSITEMFORUSECASESCENE_H
