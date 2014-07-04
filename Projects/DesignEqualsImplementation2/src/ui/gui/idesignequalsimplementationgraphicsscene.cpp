#include "idesignequalsimplementationgraphicsscene.h"

#include <QGraphicsSceneDragDropEvent>

IDesignEqualsImplementationGraphicsScene::IDesignEqualsImplementationGraphicsScene()
    : QGraphicsScene() //none of the constructors accept parent to prevent a segfault (was passing in a not fully constructed qobject as parent). Parenting is done manually in IHaveGraphicsViewAndSceneGraph's constructor anyways
{ }
IDesignEqualsImplementationGraphicsScene::IDesignEqualsImplementationGraphicsScene(const QRectF &sceneRect)
    : QGraphicsScene(sceneRect)
{ }
IDesignEqualsImplementationGraphicsScene::IDesignEqualsImplementationGraphicsScene(qreal x, qreal y, qreal width, qreal height)
    : QGraphicsScene(x, y, width, height)
{ }
IDesignEqualsImplementationGraphicsScene::~IDesignEqualsImplementationGraphicsScene()
{ }
void IDesignEqualsImplementationGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if(wantDragDropEvent(event))
    {
        event->acceptProposedAction();
    }
}
void IDesignEqualsImplementationGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if(wantDragDropEvent(event))
    {
        event->acceptProposedAction();
    }
}
void IDesignEqualsImplementationGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if(wantDragDropEvent(event)) //TODOreq: not sure if this will be a problem, but two instances of the app should not be able to share designed classes (which are only visible when on use case tab anyways). may segfault, may do nothing, idfk. for class diagram view it doesn't matter though lol
    {
        event->acceptProposedAction();
        handleAcceptedDropEvent(event);
    }
}
