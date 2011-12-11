#include "dragdropdiagramscene.h"

DragDropDiagramScene::DragDropDiagramScene(QObject *parent) :
    QGraphicsScene(parent), m_ExpectingNode(false)
{
}
void DragDropDiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //this is my hack/workaround since modesingleton's signal isn't heard
#if 0
    switch(ModeSingleton::Instance()->getMode())
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
#endif

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
        //TODOreq: the handler for this emit (even if the emit is moved elsewhere for the back-end parcelable list of nodes functionality) is what the mainmenu should listen to in order to set the buttonGroup buttons back to an unchecked state and to set the mode back to clickdragdefault. or hell, i could just set the mode back and mainwindow listens to modeChanged... but meh, i'm having trouble getting modeChanged to even be heard...
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
