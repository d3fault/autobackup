#include "designequalsimplementationclassasqgraphicsitemforclassdiagramscene.h"

#include <QPainter>
#include <QMutexLocker>

#include "../../designequalsimplementation.h"

DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene(DesignEqualsImplementationClass *designEqualsImplementationClass, QGraphicsItem *parent, Qt::WindowFlags wFlags)
    : QGraphicsWidget(parent, wFlags)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    //setFlag(QGraphicsItem::ItemSendsGeometryChanges, true); //for redrawing arrows when the item moves
    //setBrush(Qt::white);
}
QRectF DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::boundingRect() const
{
    return QGraphicsWidget::boundingRect();
    //TODOreq
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setBrush(Qt::white);
    Q_UNUSED(option)
    Q_UNUSED(widget)
    QRectF resizeBoundingRectTo;
    painter->drawText(boundingRect(), 0, "fuck\nyou", &resizeBoundingRectTo);
    /*QRectF boundingRectWithCorrectHeightAfterOneLineOfTextDrawn = resizeBoundingRectTo;
    boundingRectWithCorrectHeightAfterOneLineOfTextDrawn.setTop(resizeBoundingRectTo.height());
    painter->drawText(boundingRectWithCorrectHeightAfterOneLineOfTextDrawn, 0, "you", resizeBoundingRectTo);*/
    if(boundingRect() != resizeBoundingRectTo)
        resize(resizeBoundingRectTo.size());
    //QGraphicsWidget::paint(painter, option, widget);
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::handlePropertyAdded(DesignEqualsImplementationClassProperty *)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    //TODOreq: re-paint with new property
    //TODOoptmization: when "opening" a file, tons (hundreds, possibly thousands, depending on the project) of these handle* slots will be invoked, each one triggering a repaint. I'm not sure, but actually I think that the calls to update CAN be (and are) combined. If _NOT_, I should probably do that combining myself/hackily!!
    update(boundingRect()); //TODOoptimization: if our 'thing' (property here) is added at the BOTTOM of the uml/widget, we can supply a smaller rect to update!
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::handleHasAPrivateMemberClassAdded(HasA_PrivateMemberClasses_ListEntryType)
{
    //TODOreq: eh i think the 'second' is already going to be added to the class diagram, so although we do want to draw it, we don't connect to it's signals (or maybe we DO because that is reactor pattern at it's finest??)
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::handlePrivateMethodAdded(DesignEqualsImplementationClassPrivateMethod *)
{
    //hmm starting to think i should have just emitted the lists themselves, since i'm now thinking i am going to store them in lists also xD
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::handleSlotAdded(DesignEqualsImplementationClassSlot*)
{

}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::handleSignalAdded(DesignEqualsImplementationClassSignal *)
{

}
