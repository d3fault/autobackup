#include "designequalsimplementationexistinsignalgraphicsitemforusecasescene.h"

#include <QPen>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>

#include "designequalsimplementationguicommon.h"
#include "../../designequalsimplementationclasssignal.h"

#define DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS 2.5

//TODOreq: snap to 'notch'. Ideally we could daisy chain infinite signals together, but the more common case and much more important is to allow a list of slots to be connected to the signal
//TODOoptional: maybe all the lines share a width, such as the 'longest width (from longest signal name)'. There are some lines that go to the same class lifeline, and some that go to other class lifelines. The width measuring would be per-classlifeline ofc. Slightly OT: I do plan on doing auto-gui, which means I don't need to serialize positions of anything (maybe class diagram view can be placed manually, but I'm leaning against use case manual placing. Placing objects so they can be read is a very time consuming task and is more or less a waste of fucking time. auto-arranging is ez tbh
//TODOoptional: when doing auto-arranging stuff, say if you rename a signal name and that drastically shortens the horizontal distance from one classlifeline to the next (assuming it has a slot going to it ofc), then we should BEFORE find the center-most item on the screen, do the rename + auto-arranging, and then AFTER center the graphics view back on that center-most item. The benefit of this is that what they were working on should still be right in front of them, and not potentially (worst case) lots of scroll screens away
DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene::DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene(DesignEqualsImplementationClassSignal *theSignal, QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
{
    QPen myPen = pen();
    myPen.setWidth(3);
    myPen.setColor(Qt::blue);
    setPen(myPen);

    QGraphicsTextItem *signalNameTextGraphicsItem = new QGraphicsTextItem(theSignal->Name, this); //TODOoptional: visualize args of signal too? maybe not to keep it brief, since i'm setting the width of the arrow by it
    QRectF myChildrenBoundingRect = childrenBoundingRect();

    //TODOreq: draw differently depending on whether or not there is one slot connected, and of course account for multiple slots
    //TODOreq: we are a line, but the other components of the signal are children of us and can be any shape. we set it's pos relative to us though
    qreal startX = 0.0;
    if(parentItem()) //this seems hacky, but the parent calling setPos on us (a line) apparently doesn't work. i think as an exception to the rule, lines are "positioned" via their setLine
    {
        startX = parentItem()->boundingRect().width()/2; //TODOoptional: if the signal is being emitted to "the left" this would be negative
    }

    //make line connect at the right edge of the slot
    signalNameTextGraphicsItem->setPos(startX, -myChildrenBoundingRect.height());

    //move text to be 'above' the line
    setLine(QLineF(QPointF(startX, 0), QPointF(myChildrenBoundingRect.width()+DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_LINE_WIDTH_MARGIN_AROUND_SIGNAL_NAME_TEXT, 0)));

    QRectF slotCircleNotchGraphicsItemRect(-DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS, -DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS, DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS*2, DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS*2);
    QGraphicsEllipseItem *slotCircleNotchGraphicsItem = new QGraphicsEllipseItem(slotCircleNotchGraphicsItemRect, this);
    slotCircleNotchGraphicsItem->setPos(this->line().p2().x(), 0);
    slotCircleNotchGraphicsItem->setPen(myPen);
    slotCircleNotchGraphicsItem->setBrush(Qt::blue);
}
int DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene::type() const
{
    return DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ExistingSignal_GRAPHICS_TYPE_ID;
}
