#include "classdiagramgraphicsscene.h"

#include <QGraphicsSceneDragDropEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDataStream>

#include "designequalsimplementationguicommon.h"

ClassDiagramGraphicsScene::ClassDiagramGraphicsScene()
    : IDesignEqualsImplementationGraphicsScene()
{ }
ClassDiagramGraphicsScene::ClassDiagramGraphicsScene(const QRectF &sceneRect)
    : IDesignEqualsImplementationGraphicsScene(sceneRect)
{ }
ClassDiagramGraphicsScene::ClassDiagramGraphicsScene(qreal x, qreal y, qreal width, qreal height)
    : IDesignEqualsImplementationGraphicsScene(x, y, width, height)
{ }
ClassDiagramGraphicsScene::~ClassDiagramGraphicsScene()
{ }
void ClassDiagramGraphicsScene::handleAcceptedDropEvent(QGraphicsSceneDragDropEvent *event)
{
    QByteArray umlItemData = event->mimeData()->data(DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_CLASS_DIAGRAM_OBJECT);
    QDataStream dataStream(&umlItemData, QIODevice::ReadOnly);
    UmlItemsTypedef umlItemType;
    dataStream >> umlItemType;
    emit addUmlItemRequested(umlItemType, event->scenePos());
}
bool ClassDiagramGraphicsScene::wantDragDropEvent(QGraphicsSceneDragDropEvent *event)
{
    return (event->dropAction() == Qt::LinkAction && event->mimeData()->hasFormat(DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_CLASS_DIAGRAM_OBJECT));
}
