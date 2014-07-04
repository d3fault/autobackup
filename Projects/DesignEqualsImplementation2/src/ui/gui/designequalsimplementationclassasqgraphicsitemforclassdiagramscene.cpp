#include "designequalsimplementationclassasqgraphicsitemforclassdiagramscene.h"

#include <QPainter>
#include <QPointF>
#include <QMutexLocker>

#include "../../designequalsimplementation.h"

#define DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT 5

DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene(DesignEqualsImplementationClass *designEqualsImplementationClass, QGraphicsItem *parent, Qt::WindowFlags wFlags)
    : QGraphicsWidget(parent, wFlags)
    , m_DesignEqualsImplementationClass(designEqualsImplementationClass)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    m_ClassBorderPen.setWidth(2);
    m_LinesInBetweenLinesOfTextPen.setWidth(0);
    //setFlag(QGraphicsItem::ItemSendsGeometryChanges, true); //for redrawing arrows when the item moves
}
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
    Q_FOREACH(HasA_PrivateMemberClasses_ListEntryType currentHasA_PrivateMemberClasses_ListEntryType, m_DesignEqualsImplementationClass->HasA_PrivateMemberClasses)
    {
        classContentsString.append("\n-  " + currentHasA_PrivateMemberClasses_ListEntryType.second->ClassName + " *" + currentHasA_PrivateMemberClasses_ListEntryType.first);
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
    roundedRect.setLeft(roundedRect.left()-(DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT));
    roundedRect.setTop(roundedRect.top()-(DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT));
    roundedRect.setWidth(roundedRect.width()+(DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT));
    roundedRect.setHeight(roundedRect.height()+(DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT));
    painter->drawRoundedRect(roundedRect, 5, 5);


    //Calculate line spacing in between each line-of-text
    qreal vertialSpaceBetweenEachLineDrawn = resizeBoundingRectTo.height() / static_cast<qreal>(numLinesOfText);
    QPointF leftPointOfLine;
    leftPointOfLine.setX(resizeBoundingRectTo.left()-(DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT-1));
    leftPointOfLine.setY(resizeBoundingRectTo.top()+vertialSpaceBetweenEachLineDrawn);
    QPointF rightPointOfLine;
    rightPointOfLine.setX(resizeBoundingRectTo.right()+(DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT-1));
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
    if(!m_DesignEqualsImplementationClass->HasA_PrivateMemberClasses.isEmpty())
    {
        indexesInto_i_inForLoopToActuallyDrawLinesFor.append(indexesInto_i_inForLoopToActuallyDrawLinesFor.last()+m_DesignEqualsImplementationClass->HasA_PrivateMemberClasses.size());
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
