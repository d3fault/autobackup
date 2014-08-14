#include "designequalsimplementationclassasqgraphicsitemforclassdiagramscene.h"


#include <QGraphicsTextItem>
#include <QPointF>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>

#include <QMutexLocker>
#include "../../designequalsimplementation.h"

#include "designequalsimplementationguicommon.h"
#include"classeditordialog.h"

//TODOmb: considering changing this to a qpixmap in a graphics scene instead, where teh pixmap is drawn only when the class changes, update called once, then it's simply provided to qgraphicsview (svg might be more optimized?)... i'm going to wait on making a decision until i try to reuse the code for getting the "uml class" drag drop thingo to use the same shape (in designEquals1, i rendered to pixmap for that). i do know one thing, what i'm doing now is hella laggy (but works so fuck it)
DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene(DesignEqualsImplementationClass *designEqualsImplementationClass, QGraphicsItem *graphicsParent, QObject *qobjectParent)
    : QObject(qobjectParent)
    , QGraphicsRectItem(graphicsParent)
    , m_DesignEqualsImplementationClass(designEqualsImplementationClass)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    m_ClassBorderPen.setWidth(2);
    m_LinesInBetweenLinesOfTextPen.setWidth(0);
    //setFlag(QGraphicsItem::ItemSendsGeometryChanges, true); //for redrawing arrows when the item moves
    //setCacheMode(QGraphicsItem::DeviceCoordinateCache, QSize(500, 500));
    /*QPen myPen;
    myPen.setWidth(1);
    myPen.setBrush(Qt::transparent);
    setPen(myPen);*/
    //setBrush(Qt::white);
    setBrush(Qt::transparent); //TODOoptional: can make them white now that I'm not overriding paint event, but then need Z ordering to be [de-]serialized and modifiable in gui

    //QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);

    m_ClassContentsGraphicsTextItem = new QGraphicsTextItem(this);
    updateClassContentsGraphicsTextItem();
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *classEditorAction = menu.addAction(tr("Class Editor"));
    QAction *removeClassFromProjectAction = menu.addAction(tr("Remove Class From Project"));
    QAction *selectedAction = menu.exec(event->screenPos());
    if(!selectedAction)
        return;
    if(selectedAction == classEditorAction)
    {
        ClassEditorDialog classEditorDialog(m_DesignEqualsImplementationClass);
        if(classEditorDialog.exec() != QDialog::Accepted)
            return;
    }
    else if(selectedAction == removeClassFromProjectAction) //etc
    {
        //TODOreq
    }
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    ClassEditorDialog classEditorDialog(m_DesignEqualsImplementationClass);
    if(classEditorDialog.exec() != QDialog::Accepted)
        return;
}
QString DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::classDetailsAsHtmlString()
{
    QString classContentsString("<b>" + m_DesignEqualsImplementationClass->ClassName + "</b>"); //TODOoptional: figure out out how to center this in addition to bolding it (<center> didn't work)
    int numLinesOfText = 1;

#if 0
    //QString newLine = "<br />";

    bool publicAccessorSpecified = false; //in case we do public methods too
    if(!m_DesignEqualsImplementationClass->Properties.isEmpty() && !publicAccessorSpecified)
    {
        classContentsString.append("<br /><br />public:");
        publicAccessorSpecified = true;
    }
    Q_FOREACH(DesignEqualsImplementationClassProperty *currentProperty, m_DesignEqualsImplementationClass->Properties)
    {
        classContentsString.append("<br />Q_PROPERTY(" + m_DesignEqualsImplementationClass->ClassName + "::" + currentProperty->Type + "::" + currentProperty->Name + ");");
        ++numLinesOfText;
    }
    bool privateAccessorSpecified = false;
    if(!m_DesignEqualsImplementationClass->HasA_Private_Classes_Members.isEmpty() && !privateAccessorSpecified)
    {
        classContentsString.append("<br /><br />private:");
        privateAccessorSpecified = true;
    }
    Q_FOREACH(HasA_Private_Classes_Members_ListEntryType *currentHasA_Private_Classes_Members_ListEntryType, m_DesignEqualsImplementationClass->HasA_Private_Classes_Members)
    {
        classContentsString.append("<br />" + currentHasA_Private_Classes_Members_ListEntryType->preferredTextualRepresentation() + ";");
        ++numLinesOfText;
    }
    if(!m_DesignEqualsImplementationClass->PrivateMethods.isEmpty() && !privateAccessorSpecified)
    {
        classContentsString.append("<br /><br />private:");
        privateAccessorSpecified = true;
    }
    Q_FOREACH(DesignEqualsImplementationClassPrivateMethod *currentPrivateMethod, m_DesignEqualsImplementationClass->PrivateMethods)
    {
        classContentsString.append("<br />void " + currentPrivateMethod->Name + "();"); //TODOreq: remove parenthesis hack, use methodSignature properly
        ++numLinesOfText;
    }
    if(!m_DesignEqualsImplementationClass->Signals.isEmpty())
    {
        classContentsString.append("<br /><br />signals:");
    }
    Q_FOREACH(DesignEqualsImplementationClassSignal *currentSignal, m_DesignEqualsImplementationClass->Signals)
    {
        classContentsString.append("<br />void " + currentSignal->methodSignatureWithoutReturnType() + ";");
        ++numLinesOfText;
    }
    if(!m_DesignEqualsImplementationClass->Slots.isEmpty())
    {
        classContentsString.append("<br /><br />public slots:");
    }
    Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, m_DesignEqualsImplementationClass->Slots)
    {
        classContentsString.append("<br />void " + currentSlot->methodSignatureWithoutReturnType() + ";");
        ++numLinesOfText;
    }
#endif
    Q_FOREACH(DesignEqualsImplementationClassProperty *currentProperty, m_DesignEqualsImplementationClass->Properties)
    {
        classContentsString.append("<br />o  " + currentProperty->Type + ": " + currentProperty->Name);
        ++numLinesOfText;
    }
    Q_FOREACH(HasA_Private_Classes_Member *currentHasA_Private_Classes_Members_ListEntryType, m_DesignEqualsImplementationClass->hasA_Private_Classes_Members())
    {
        classContentsString.append("<br />-  " + currentHasA_Private_Classes_Members_ListEntryType->preferredTextualRepresentationOfTypeAndVariableTogether());
        ++numLinesOfText;
    }
    Q_FOREACH(DesignEqualsImplementationClassPrivateMethod *currentPrivateMethod, m_DesignEqualsImplementationClass->PrivateMethods)
    {
        classContentsString.append("<br />-  " + currentPrivateMethod->Name); //TODOreq: methodSignature
        ++numLinesOfText;
    }
    Q_FOREACH(DesignEqualsImplementationClassSignal *currentSignal, m_DesignEqualsImplementationClass->mySignals())
    {
        classContentsString.append("<br />)) " + currentSignal->methodSignatureWithoutReturnType());
        ++numLinesOfText;
    }
    Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, m_DesignEqualsImplementationClass->mySlots())
    {
        classContentsString.append("<br />+  " + currentSlot->methodSignatureWithoutReturnType());
        ++numLinesOfText;
    }
    return classContentsString;
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::updateClassContentsGraphicsTextItem()
{
    m_ClassContentsGraphicsTextItem->setHtml(classDetailsAsHtmlString());
    setRect(childrenBoundingRect());
}
#if 0
QRectF DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::boundingRect() const
{
    return m_BoundingRect;
    //return QGraphicsWidget::boundingRect();
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    //TODOreq: this scopedLock must be commented out for click drag to work AT ALL hahahaha... I need a better synchronization solution. Right now it's not thread safe (but eh idc for now)
    //QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex); //rofl. of all places that should use a faster method (implicit sharing, or even manual synchronization :-P)

    painter->save();
    //painter->setRenderHint(QPainter::Antialiasing, true); //TO DOneoptimization: maybe i can specify this somewhere else so i don't have to keep re-specifying it. also noticed noticeable slowdown when enabled lolol, BUT the level of sexiness hit maximum too. //TODOoptional: run-time option obviously makes a lot of sense
    painter->setBrush(Qt::transparent); //would do white, but i have to draw text first in order to get my bounding rect starting point guh, so a white fill on the rounded rect afterwards covers the words! fml
    painter->setPen(m_ClassBorderPen);
    QString classContentsString(m_DesignEqualsImplementationClass->ClassName);
    int numLinesOfText = 1;
    Q_FOREACH(DesignEqualsImplementationClassProperty *currentProperty, m_DesignEqualsImplementationClass->Properties)
    {
        classContentsString.append("\no  " + currentProperty->Name);
        ++numLinesOfText;
    }
    Q_FOREACH(HasA_Private_Classes_Members_ListEntryType *currentHasA_Private_Classes_Members_ListEntryType, m_DesignEqualsImplementationClass->HasA_Private_Classes_Members)
    {
        classContentsString.append("\n-  " + currentHasA_Private_Classes_Members_ListEntryType->preferredTextualRepresentation());
        ++numLinesOfText;
    }
    Q_FOREACH(DesignEqualsImplementationClassPrivateMethod *currentPrivateMethod, m_DesignEqualsImplementationClass->PrivateMethods)
    {
        classContentsString.append("\n-  " + currentPrivateMethod->Name); //TODOreq: methodSignature
        ++numLinesOfText;
    }
    Q_FOREACH(DesignEqualsImplementationClassSignal *currentSignal, m_DesignEqualsImplementationClass->Signals)
    {
        classContentsString.append("\n)) " + currentSignal->methodSignatureWithoutReturnType());
        ++numLinesOfText;
    }
    Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, m_DesignEqualsImplementationClass->Slots)
    {
        classContentsString.append("\n+  " + currentSlot->methodSignatureWithoutReturnType());
        ++numLinesOfText;
    }
    QRectF resizeBoundingRectTo;
    painter->drawText(boundingRect(), (Qt::AlignVCenter /*| Qt::TextDontClip*/), classContentsString, &resizeBoundingRectTo);
    QRectF roundedRect = resizeBoundingRectTo;
    roundedRect.setLeft(roundedRect.left()-(DESIGNEQUALSIMPLEMENTATION_GUI_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT));
    roundedRect.setTop(roundedRect.top()-(DESIGNEQUALSIMPLEMENTATION_GUI_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT));
    roundedRect.setWidth(roundedRect.width()+(DESIGNEQUALSIMPLEMENTATION_GUI_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT));
    roundedRect.setHeight(roundedRect.height()+(DESIGNEQUALSIMPLEMENTATION_GUI_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT));
    painter->drawRoundedRect(roundedRect, DESIGNEQUALSIMPLEMENTATION_GUI_CLASS_GRAPHICS_ITEM_ROUNDED_RECTANGLE_RADIUS, DESIGNEQUALSIMPLEMENTATION_GUI_CLASS_GRAPHICS_ITEM_ROUNDED_RECTANGLE_RADIUS);


    //Calculate line spacing in between each line-of-text
    qreal vertialSpaceBetweenEachLineDrawn = resizeBoundingRectTo.height() / static_cast<qreal>(numLinesOfText);
    QPointF leftPointOfLine;
    leftPointOfLine.setX(resizeBoundingRectTo.left()-(DESIGNEQUALSIMPLEMENTATION_GUI_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT-1));
    leftPointOfLine.setY(resizeBoundingRectTo.top()+vertialSpaceBetweenEachLineDrawn);
    QPointF rightPointOfLine;
    rightPointOfLine.setX(resizeBoundingRectTo.right()+(DESIGNEQUALSIMPLEMENTATION_GUI_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT-1));
    rightPointOfLine.setY(resizeBoundingRectTo.top()+vertialSpaceBetweenEachLineDrawn);
    bool drawingFirstLine = true; //Line between class name and rest should be same as border width
    --numLinesOfText; //This might look like an off by one, but I decided not to draw the very last/bottom line because it looks retarded

    //Only draw lines between class sub-types (signals/slots/private-methods/etc)
    QList<int> indexesInto_i_inForLoopToActuallyDrawLinesFor; //we still need to know the position of all lines for proper spacing
    //int runningIforMarkingLinesToDrawSynchronizedWithLinesDeterminedForSpacing = 0;
    //indexesInto_i_inForLoopToActuallyDrawLinesFor.append(runningIforMarkingLinesToDrawSynchronizedWithLinesDeterminedForSpacing++);
    indexesInto_i_inForLoopToActuallyDrawLinesFor.append(0); //first line after class name
    if(!m_DesignEqualsImplementationClass->Properties.isEmpty())
    {
        indexesInto_i_inForLoopToActuallyDrawLinesFor.append(indexesInto_i_inForLoopToActuallyDrawLinesFor.last()+m_DesignEqualsImplementationClass->Properties.size());
    }
    if(!m_DesignEqualsImplementationClass->HasA_Private_Classes_Members.isEmpty())
    {
        indexesInto_i_inForLoopToActuallyDrawLinesFor.append(indexesInto_i_inForLoopToActuallyDrawLinesFor.last()+m_DesignEqualsImplementationClass->HasA_Private_Classes_Members.size());
    }
    if(!m_DesignEqualsImplementationClass->PrivateMethods.isEmpty())
    {
        indexesInto_i_inForLoopToActuallyDrawLinesFor.append(indexesInto_i_inForLoopToActuallyDrawLinesFor.last()+m_DesignEqualsImplementationClass->PrivateMethods.size());
    }
    if(!m_DesignEqualsImplementationClass->Signals.isEmpty())
    {
        indexesInto_i_inForLoopToActuallyDrawLinesFor.append(indexesInto_i_inForLoopToActuallyDrawLinesFor.last()+m_DesignEqualsImplementationClass->Signals.size());
    }
    if(!m_DesignEqualsImplementationClass->Slots.isEmpty())
    {
        indexesInto_i_inForLoopToActuallyDrawLinesFor.append(indexesInto_i_inForLoopToActuallyDrawLinesFor.last()+m_DesignEqualsImplementationClass->Slots.size());
    }


    for(int i = 0; i < numLinesOfText; ++i)
    {
        if(indexesInto_i_inForLoopToActuallyDrawLinesFor.contains(i))
            painter->drawLine(leftPointOfLine, rightPointOfLine);
        leftPointOfLine.setY(leftPointOfLine.y()+vertialSpaceBetweenEachLineDrawn);
        rightPointOfLine.setY(leftPointOfLine.y()); //they are even
        if(drawingFirstLine)
        {
            painter->setPen(m_LinesInBetweenLinesOfTextPen);;
            drawingFirstLine = false;
        }
    }

    m_BoundingRect = resizeBoundingRectTo;
    painter->restore();
}
#endif
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::handlePropertyAdded(DesignEqualsImplementationClassProperty *propertyAdded)
{
    Q_UNUSED(propertyAdded)

    //QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    //TODOreq: re-paint with new property
    //TODOoptmization: when "opening" a file, tons (hundreds, possibly thousands, depending on the project) of these handle* slots will be invoked, each one triggering a repaint. I'm not sure, but actually I think that the calls to update CAN be (and are) combined. If _NOT_, I should probably do that combining myself/hackily!!
    //update(boundingRect()); //TODOoptimization: if our 'thing' (property here) is added at the BOTTOM of the uml/widget, we can supply a smaller rect to update!

    updateClassContentsGraphicsTextItem(); //TODOoptimization: don't update EVERYTHING, just append the new property
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::handleHasAPrivateMemberClassAdded(HasA_Private_Classes_Member *)
{
    //TODOreq: eh i think the 'second' is already going to be added to the class diagram, so although we do want to draw it, we don't connect to it's signals (or maybe we DO because that is reactor pattern at it's finest??)
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::handlePrivateMethodAdded(DesignEqualsImplementationClassPrivateMethod *)
{
    //hmm starting to think i should have just emitted the lists themselves, since i'm now thinking i am going to store them in lists also xD
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::handleSlotAdded(DesignEqualsImplementationClassSlot*)
{
    //TODOprobably (same as the other handles, so really they can all be merged...):
    //myPaint(); //does draw incl mutex lock
    //update(); //paint works on static item (pixmap?)
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::handleSignalAdded(DesignEqualsImplementationClassSignal *)
{

}
