#include "designequalsimplementationclassasqgraphicsitemforclassdiagramscene.h"

#include <QPainter>
#include <QMutexLocker>

#include "../../designequalsimplementation.h"

DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene(DesignEqualsImplementationClass *designEqualsImplementationClass, QGraphicsItem *parent, Qt::WindowFlags wFlags)
    : QGraphicsWidget(parent, wFlags)
    , m_DesignEqualsImplementationClass(designEqualsImplementationClass)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    //setFlag(QGraphicsItem::ItemSendsGeometryChanges, true); //for redrawing arrows when the item moves
}
QRectF DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::boundingRect() const
{
    return m_BoundingRect;
    //return QGraphicsWidget::boundingRect();
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex); //rofl. of all places that should use a faster method (implicit sharing, or even manual synchronization :-P)

    painter->save();
    painter->setBrush(Qt::transparent); //would do white, but i have to draw text first in order to get my bounding rect starting point guh, so a white fill on the rounded rect afterwards covers the words! fml
    QPen myPen;
    myPen.setWidth(2);
    painter->setPen(myPen);
    Q_UNUSED(option)
    Q_UNUSED(widget)
    QString classContentsString(m_DesignEqualsImplementationClass->ClassName);
    Q_FOREACH(DesignEqualsImplementationClassProperty *currentProperty, m_DesignEqualsImplementationClass->Properties)
    {
        classContentsString.append("\no  " + currentProperty->Name);
    }
    Q_FOREACH(HasA_PrivateMemberClasses_ListEntryType currentHasA_PrivateMemberClasses_ListEntryType, m_DesignEqualsImplementationClass->HasA_PrivateMemberClasses)
    {
        classContentsString.append("\n-  " + currentHasA_PrivateMemberClasses_ListEntryType.second->ClassName + " *" + currentHasA_PrivateMemberClasses_ListEntryType.first);
    }
    Q_FOREACH(DesignEqualsImplementationClassPrivateMethod *currentPrivateMethod, m_DesignEqualsImplementationClass->PrivateMethods)
    {
        classContentsString.append("\n-  " + currentPrivateMethod->Name); //TODOreq: methodSignature
    }
    Q_FOREACH(DesignEqualsImplementationClassSignal *currentSignal, m_DesignEqualsImplementationClass->Signals)
    {
        classContentsString.append("\n)) " + currentSignal->methodSignatureWithoutReturnType());
    }
    Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, m_DesignEqualsImplementationClass->Slots)
    {
        classContentsString.append("\n+  " + currentSlot->methodSignatureWithoutReturnType());
    }
    QRectF resizeBoundingRectTo;
    painter->drawText(boundingRect(), (Qt::AlignVCenter /*| Qt::TextDontClip*/), classContentsString, &resizeBoundingRectTo);
    /*QRectF boundingRectWithCorrectHeightAfterOneLineOfTextDrawn = resizeBoundingRectTo;
    boundingRectWithCorrectHeightAfterOneLineOfTextDrawn.setTop(resizeBoundingRectTo.height());
    painter->drawText(boundingRectWithCorrectHeightAfterOneLineOfTextDrawn, 0, "you", resizeBoundingRectTo);*/
    //resizeBoundingRectTo.setHeight(resizeBoundingRectTo.height()+(10));
    //resizeBoundingRectTo.setWidth(resizeBoundingRectTo.height()+(10));
    //resizeBoundingRectTo.setLeft(resizeBoundingRectTo.left()-10);
    painter->drawRoundedRect(resizeBoundingRectTo, 5, 5);
    //resizeBoundingRectTo.setHeight(resizeBoundingRectTo.height()+5);
    //resizeBoundingRectTo.setWidth(resizeBoundingRectTo.width()+5);
    //resizeBoundingRectTo.translate(-2.5, -2.5);
    //if(boundingRect() != resizeBoundingRectTo)
        //resize(resizeBoundingRectTo.size());
    //setGeometry(resizeBoundingRectTo);
    //QGraphicsWidget::paint(painter, option, widget);
    m_BoundingRect = resizeBoundingRectTo;
    painter->restore();
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
