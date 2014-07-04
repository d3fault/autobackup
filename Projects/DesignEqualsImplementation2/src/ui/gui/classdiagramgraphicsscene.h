#ifndef CLASSDIAGRAMGRAPHICSSCENE_H
#define CLASSDIAGRAMGRAPHICSSCENE_H

#include <QGraphicsScene>

#include "../../designequalsimplementationcommon.h"

class ClassDiagramGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    ClassDiagramGraphicsScene(QObject *parent = 0);
    ClassDiagramGraphicsScene(const QRectF &sceneRect, QObject *parent = 0);
    ClassDiagramGraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent = 0);
    virtual ~ClassDiagramGraphicsScene();
private:
    bool wantDragEvent(QGraphicsSceneDragDropEvent *event);
protected:
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event);
signals:
    void addUmlItemRequested(UmlItemsTypedef umlItemType, QPointF position);
};

#endif // CLASSDIAGRAMGRAPHICSSCENE_H
