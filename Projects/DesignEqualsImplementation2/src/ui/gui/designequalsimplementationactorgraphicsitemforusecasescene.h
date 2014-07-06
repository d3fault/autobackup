#ifndef DESIGNEQUALSIMPLEMENTATIONACTORGRAPHICSITEMFORUSECASESCENE_H
#define DESIGNEQUALSIMPLEMENTATIONACTORGRAPHICSITEMFORUSECASESCENE_H

#include <QGraphicsRectItem>

class QGraphicsSceneMouseEvent;

class DesignEqualsImplementationActor;

class DesignEqualsImplementationActorGraphicsItemForUseCaseScene : public QGraphicsRectItem
{
public:
    explicit DesignEqualsImplementationActorGraphicsItemForUseCaseScene(DesignEqualsImplementationActor *actor, QGraphicsItem *parent = 0);
    explicit DesignEqualsImplementationActorGraphicsItemForUseCaseScene(DesignEqualsImplementationActor *actor, const QRectF &rect, QGraphicsItem *parent = 0);
    explicit DesignEqualsImplementationActorGraphicsItemForUseCaseScene(DesignEqualsImplementationActor *actor, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = 0);
protected:
    //virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
private:
    DesignEqualsImplementationActor *m_Actor;

    void privateConstructor(DesignEqualsImplementationActor *actor);
};

#endif // DESIGNEQUALSIMPLEMENTATIONACTORGRAPHICSITEMFORUSECASESCENE_H
