#include "classdiagramgraphicsscene.h"

#include <QGraphicsSceneDragDropEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDataStream>

#include "designequalsimplementationguicommon.h"

ClassDiagramGraphicsScene::ClassDiagramGraphicsScene(QObject *parent)
    : QGraphicsScene(parent)
{ }
ClassDiagramGraphicsScene::ClassDiagramGraphicsScene(const QRectF &sceneRect, QObject *parent)
    : QGraphicsScene(sceneRect, parent)
{ }
ClassDiagramGraphicsScene::ClassDiagramGraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent)
    : QGraphicsScene(x, y, width, height, parent)
{ }
ClassDiagramGraphicsScene::~ClassDiagramGraphicsScene()
{ }
bool ClassDiagramGraphicsScene::wantDragEvent(QGraphicsSceneDragDropEvent *event)
{
    return (event->dropAction() == Qt::LinkAction && event->mimeData()->hasFormat(DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_CLASS_DIAGRAM_OBJECT));
}
void ClassDiagramGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if(wantDragEvent(event))
    {
        event->acceptProposedAction();
    }
}
void ClassDiagramGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if(wantDragEvent(event))
    {
        event->acceptProposedAction();
    }
}
void ClassDiagramGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if(wantDragEvent(event)) //TODOreq: not sure if this will be a problem, but two instances of the app should not be able to share designed classes (which are only visible when on use case tab anyways). may segfault, may do nothing, idfk. for class diagram view it doesn't matter though lol
    {
        QByteArray umlItemData = event->mimeData()->data(DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_CLASS_DIAGRAM_OBJECT);
        QDataStream dataStream(&umlItemData, QIODevice::ReadOnly);
        UmlItemsTypedef umlItemType;
        dataStream >> umlItemType;
        event->acceptProposedAction();
        emit addUmlItemRequested(umlItemType, event->scenePos());
    }
}
