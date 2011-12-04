#include "dragdropdiagramscene.h"

DragDropDiagramScene::DragDropDiagramScene(QObject *parent) :
    QGraphicsScene(parent)
{
}
void DragDropDiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_ExpectingNode)
    {
        item = new DiagramItem(myItemType, myItemMenu);
        item->setBrush(myItemColor);
        addItem(item);
        item->setPos(event->scenePos());

        emit itemInserted(item);
    }
}
