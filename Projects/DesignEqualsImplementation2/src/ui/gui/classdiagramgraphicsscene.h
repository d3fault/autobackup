#ifndef CLASSDIAGRAMGRAPHICSSCENE_H
#define CLASSDIAGRAMGRAPHICSSCENE_H

#include "idesignequalsimplementationgraphicsscene.h"
#include "../../designequalsimplementationcommon.h"

class ClassDiagramGraphicsScene : public IDesignEqualsImplementationGraphicsScene
{
    Q_OBJECT
public:
    ClassDiagramGraphicsScene();
    ClassDiagramGraphicsScene(const QRectF &sceneRect);
    ClassDiagramGraphicsScene(qreal x, qreal y, qreal width, qreal height);
    virtual ~ClassDiagramGraphicsScene();
protected:
    virtual void handleAcceptedDropEvent(QGraphicsSceneDragDropEvent *event);
private:
    virtual bool wantDragDropEvent(QGraphicsSceneDragDropEvent *event);
signals:
    void addUmlItemRequested(UmlItemsTypedef umlItemType, QPointF position);
};

#endif // CLASSDIAGRAMGRAPHICSSCENE_H
