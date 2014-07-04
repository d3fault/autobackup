#ifndef IDESIGNEQUALSIMPLEMENTATIONGRAPHICSSCENE_H
#define IDESIGNEQUALSIMPLEMENTATIONGRAPHICSSCENE_H

#include <QGraphicsScene>

class QGraphicsSceneDragDropEvent;

class IDesignEqualsImplementationGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    IDesignEqualsImplementationGraphicsScene();
    IDesignEqualsImplementationGraphicsScene(const QRectF &sceneRect);
    IDesignEqualsImplementationGraphicsScene(qreal x, qreal y, qreal width, qreal height);
    virtual ~IDesignEqualsImplementationGraphicsScene();
protected:
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event);

    virtual void handleAcceptedDropEvent(QGraphicsSceneDragDropEvent *event)=0;
private:
    virtual bool wantDragDropEvent(QGraphicsSceneDragDropEvent *event)=0;
};

#endif // IDESIGNEQUALSIMPLEMENTATIONGRAPHICSSCENE_H
