#ifndef USECASEGRAPHICSSCENE_H
#define USECASEGRAPHICSSCENE_H

#include "idesignequalsimplementationgraphicsscene.h"

class UseCaseGraphicsScene : public IDesignEqualsImplementationGraphicsScene
{
    Q_OBJECT
public:
    UseCaseGraphicsScene();
    UseCaseGraphicsScene(const QRectF &sceneRect);
    UseCaseGraphicsScene(qreal x, qreal y, qreal width, qreal height);
    virtual ~UseCaseGraphicsScene();
protected:
    virtual void handleAcceptedDropEvent(QGraphicsSceneDragDropEvent *event);
private:
    virtual bool wantDragDropEvent(QGraphicsSceneDragDropEvent *event);
};

#endif // USECASEGRAPHICSSCENE_H
