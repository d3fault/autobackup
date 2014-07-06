#include "designequalsimplementationactorgraphicsitemforusecasescene.h"

#include <QGraphicsSimpleTextItem>

#include "designequalsimplementationguicommon.h"
#include "usecasegraphicsscene.h"
#include "../../designequalsimplementationactor.h"

#define DesignEqualsImplementationActorGraphicsItemForUseCaseScene_SQUARE_MIN_SIDE_LENGTH static_cast<qreal>(100)

DesignEqualsImplementationActorGraphicsItemForUseCaseScene::DesignEqualsImplementationActorGraphicsItemForUseCaseScene(DesignEqualsImplementationActor *actor, QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
{
    privateConstructor(actor);
}
DesignEqualsImplementationActorGraphicsItemForUseCaseScene::DesignEqualsImplementationActorGraphicsItemForUseCaseScene(DesignEqualsImplementationActor *actor, const QRectF &rect, QGraphicsItem *parent)
    : QGraphicsRectItem(rect, parent)
{
    privateConstructor(actor);
}
DesignEqualsImplementationActorGraphicsItemForUseCaseScene::DesignEqualsImplementationActorGraphicsItemForUseCaseScene(DesignEqualsImplementationActor *actor, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent)
    : QGraphicsRectItem(x, y, w, h, parent)
{
    privateConstructor(actor);
}
#if 0
void DesignEqualsImplementationActorGraphicsItemForUseCaseScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(static_cast<UseCaseGraphicsScene*>(scene())->mouseMode() == DesignEqualsImplementationMouseDrawSignalSlotConnectionActivationArrowsMode)
    {
        //TODOreq: don't move, begin arrow draw
        setFlag(QGraphicsItem::ItemIsMovable, false);
    }
    else
    {
        //move
        setFlag(QGraphicsItem::ItemIsMovable, true);
    }
}
#endif
void DesignEqualsImplementationActorGraphicsItemForUseCaseScene::privateConstructor(DesignEqualsImplementationActor *actor)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    m_Actor = actor;

    QGraphicsSimpleTextItem *actorText = new QGraphicsSimpleTextItem(QObject::tr(DESIGN_EQUALS_IMPLEMENTATION_ACTOR_STRING), this);

    //enforce minimum rect size around text
    QRectF actorTextRect = actorText->boundingRect();
    actorText->setPos(-(actorTextRect.width()/2), -(actorTextRect.height()/2));
    qreal myHeight = qMax(actorTextRect.height(), DesignEqualsImplementationActorGraphicsItemForUseCaseScene_SQUARE_MIN_SIDE_LENGTH);
    qreal myWidth = qMax(actorTextRect.width(), DesignEqualsImplementationActorGraphicsItemForUseCaseScene_SQUARE_MIN_SIDE_LENGTH);
    QPointF myTopLeft(-(myWidth/2), -(myHeight/2));
    QPointF myBottomRight(myWidth/2, myHeight/2);

    //center the actor text
    QRectF myRect(myTopLeft, myBottomRight);

    setRect(myRect);
    //TODOreq: draw a bitchin stick figure guy (make it resemble "1.jpg" (or shit trace 1.jpg into an svg xD (me sitting in the chair facing the right is perfect!))) using QPainterPath with 'this' as parent
}
