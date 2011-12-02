#include "dragdropdiagramscene.h"

DragDropDiagramScene::DragDropDiagramScene(QObject *parent) :
    QGraphicsScene(parent)
{
}
void DragDropDiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //
    /*
    if(expectingNode())
    {
        item = new DiagramItem(myItemType, myItemMenu);
        item->setBrush(myItemColor);
        addItem(item);
        item->setPos(event->scenePos());

        emit itemInserted(item);
    }
    */
}
bool DragDropDiagramScene::expectingNode()
{
    //TODOopt: not sure if i should pull from the mainmenu mode the answer of if i should listen to a signal emitted by mainmenu and sync a local accordingly. not sure it even matters.
}
