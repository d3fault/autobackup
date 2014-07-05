#ifndef CLASSDIAGRAMGRAPHICSSCENE_H
#define CLASSDIAGRAMGRAPHICSSCENE_H

#include "idesignequalsimplementationgraphicsscene.h"
#include "../../designequalsimplementationcommon.h"

class DesignEqualsImplementationProject;
class DesignEqualsImplementationClass;

class ClassDiagramGraphicsScene : public IDesignEqualsImplementationGraphicsScene
{
    Q_OBJECT
public:
    ClassDiagramGraphicsScene(DesignEqualsImplementationProject *designEqualsImplementationProject);
    ClassDiagramGraphicsScene(DesignEqualsImplementationProject *designEqualsImplementationProject, const QRectF &sceneRect);
    ClassDiagramGraphicsScene(DesignEqualsImplementationProject *designEqualsImplementationProject, qreal x, qreal y, qreal width, qreal height);
    virtual ~ClassDiagramGraphicsScene();
protected:
    virtual void handleAcceptedDropEvent(QGraphicsSceneDragDropEvent *event);
private:
    void privateConstructor(DesignEqualsImplementationProject *designEqualsImplementationProject);
    virtual bool wantDragDropEvent(QGraphicsSceneDragDropEvent *event);
signals:
    void addUmlItemRequested(UmlItemsTypedef umlItemType, QPointF position);
private slots:
    void handleClassAdded(DesignEqualsImplementationClass *classAdded);
};

#endif // CLASSDIAGRAMGRAPHICSSCENE_H
