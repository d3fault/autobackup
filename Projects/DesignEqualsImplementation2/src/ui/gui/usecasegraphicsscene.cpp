#include "usecasegraphicsscene.h"

#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>

#include "designequalsimplementationguicommon.h"

UseCaseGraphicsScene::UseCaseGraphicsScene()
    : IDesignEqualsImplementationGraphicsScene()
{ }
UseCaseGraphicsScene::UseCaseGraphicsScene(const QRectF &sceneRect)
    : IDesignEqualsImplementationGraphicsScene(sceneRect)
{ }
UseCaseGraphicsScene::UseCaseGraphicsScene(qreal x, qreal y, qreal width, qreal height)
    : IDesignEqualsImplementationGraphicsScene(x, y, width, height)
{ }
UseCaseGraphicsScene::~UseCaseGraphicsScene()
{ }
void UseCaseGraphicsScene::handleAcceptedDropEvent(QGraphicsSceneDragDropEvent *event)
{
    //TODOreq
}
bool UseCaseGraphicsScene::wantDragDropEvent(QGraphicsSceneDragDropEvent *event)
{
    return (event->dropAction() == Qt::LinkAction && event->mimeData()->hasFormat(DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_USE_CASE_OBJECT));
}
