#include "dragdropdiagramscene.h"

DragDropDiagramScene::DragDropDiagramScene(QObject *parent) :
    QGraphicsScene(parent), m_ExpectingNode(false)
{
}
void DragDropDiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_ExpectingNode)
    {
        //button unique id is used to get the template copy
        //TODOopt: should be copy-on-write, for now since i won't even be changing anything, just use the template lol.. but soon going to need to do a copy-on-read prolly
        //the special type is only known locally as a DiagramSceneNode*, it's virtual functions defined appropriately
        //TODOreq: also need to add it to some back-end parcelable list (which should emit a signal that THEN triggers the drawing/adding to the scene)

        DiagramSceneNode *nodeToAdd = ModeSingleton::Instance()->getPendingNode();

        //item = new DiagramItem(myItemType, myItemMenu);
        //item->setBrush(myItemColor);
        this->addItem(nodeToAdd);
        nodeToAdd->setPos(event->scenePos());

        //emit itemInserted(item);
    }
}
void DragDropDiagramScene::handleModeChanged(ModeSingleton::Mode newMode)
{
    switch(newMode)
    {
    case ModeSingleton::ClickDragDefaultMode:
        m_ExpectingNode = false;
        break;
    case ModeSingleton::AddNodeMode:
        m_ExpectingNode = true;
        break;
    default:
        //set all to false
        m_ExpectingNode = false;
    }
}
