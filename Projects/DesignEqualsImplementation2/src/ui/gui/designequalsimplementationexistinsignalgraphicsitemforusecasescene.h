#ifndef DESIGNEQUALSIMPLEMENTATIONEXISTINSIGNALGRAPHICSITEMFORUSECASESCENE_H
#define DESIGNEQUALSIMPLEMENTATIONEXISTINSIGNALGRAPHICSITEMFORUSECASESCENE_H

#include <QGraphicsItem>

class DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene : public QGraphicsItem
{
public:
    explicit DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene(QGraphicsItem *parent = 0);
    virtual ~DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene() { }
    virtual int type() const;
};

#endif // DESIGNEQUALSIMPLEMENTATIONEXISTINSIGNALGRAPHICSITEMFORUSECASESCENE_H
