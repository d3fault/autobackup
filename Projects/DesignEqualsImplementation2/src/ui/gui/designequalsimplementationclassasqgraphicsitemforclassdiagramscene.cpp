#include "designequalsimplementationclassasqgraphicsitemforclassdiagramscene.h"


#include <QGraphicsTextItem>
#include <QPointF>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>

#include "designequalsimplementationguicommon.h"
#include "classeditordialog.h"
#include "../../designequalsimplementationcommon.h"
#include "../../designequalsimplementationproject.h"

//TODOmb: considering changing this to a qpixmap in a graphics scene instead, where teh pixmap is drawn only when the class changes, update called once, then it's simply provided to qgraphicsview (svg might be more optimized?)... i'm going to wait on making a decision until i try to reuse the code for getting the "uml class" drag drop thingo to use the same shape (in designEquals1, i rendered to pixmap for that). i do know one thing, what i'm doing now is hella laggy (but works so fuck it)
//TODOreq: s/Class/Type
DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene(DesignEqualsImplementationType *designEqualsImplementationClass, DesignEqualsImplementationProject *currentProject, QGraphicsItem *graphicsParent, QObject *qobjectParent)
    : QObject(qobjectParent)
    , QGraphicsRectItem(graphicsParent)
    , m_Type(designEqualsImplementationClass)
    , m_CurrentProject(currentProject)
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

    m_ClassContentsGraphicsTextItem = new QGraphicsTextItem(this);
    updateClassContentsGraphicsTextItem();

    connect(this, SIGNAL(editCppModeRequested(DesignEqualsImplementationType*)), currentProject, SLOT(handleEditCppModeRequested(DesignEqualsImplementationType*)));
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *classEditorAction = menu.addAction(tr("Class Editor"));
    QAction *editCppAction = menu.addAction(tr("Edit C++"));
    QAction *removeClassFromProjectAction = menu.addAction(tr("Remove Class From Project -- TODO"));
    QAction *selectedAction = menu.exec(event->screenPos());
    if(!selectedAction)
        return;
    if(selectedAction == classEditorAction)
    {
        ClassEditorDialog classEditorDialog(m_Type, m_CurrentProject);
        classEditorDialog.exec();
    }
    else if(selectedAction == editCppAction)
    {
        emit editCppModeRequested(m_Type);
    }
    else if(selectedAction == removeClassFromProjectAction) //etc
    {
        //TODOreq
    }
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    ClassEditorDialog classEditorDialog(m_Type, m_CurrentProject);
    classEditorDialog.exec();
}
QString DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::classDetailsAsHtmlString()
{
    QString classContentsString("<b>" + m_Type->Name + "</b>"); //TODOoptional: figure out out how to center this in addition to bolding it (<center> didn't work)
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
        classContentsString.append("<br />Q_PROPERTY(" + m_DesignEqualsImplementationClass->Name + "::" + currentProperty->type->Name + "::" + currentProperty->Name + ");");
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
    DesignEqualsImplementationClass *typeAsClass = qobject_cast<DesignEqualsImplementationClass*>(m_Type);
    Q_FOREACH(NonFunctionMember *currentNonFunctionMember, m_Type->nonFunctionMembers())
    {
        QString visibilityVisualHtml;
        if(typeAsClass) //all members of implicitly shared data types are 'public'
        {
            switch(currentNonFunctionMember->visibility)
            {
            case Visibility::Public:
                visibilityVisualHtml = "+ ";
                break;
            case Visibility::Protected: //TODOoptional: use something else
            case Visibility::Private:
                visibilityVisualHtml = "- ";
                break;
            }
        }
        classContentsString.append("<br />" + visibilityVisualHtml + currentNonFunctionMember->preferredTextualRepresentationOfTypeAndVariableTogether()); //TODOmb: try to cast to property, display it differently
        ++numLinesOfText;
    }
    if(typeAsClass)
    {
        Q_FOREACH(DesignEqualsImplementationClassPrivateMethod *currentPrivateMethod, typeAsClass->PrivateMethods)
        {
            classContentsString.append("<br />-  " + currentPrivateMethod->Name); //TODOreq: methodSignature
            ++numLinesOfText;
        }
        Q_FOREACH(DesignEqualsImplementationClassSignal *currentSignal, typeAsClass->mySignals())
        {
            classContentsString.append("<br />)) " + currentSignal->methodSignatureWithoutReturnType());
            ++numLinesOfText;
        }
        Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, typeAsClass->mySlots())
        {
            if(!currentSlot->Name.startsWith(UseCaseGraphicsScene_TEMP_SLOT_MAGICAL_NAME_STRING_PREFIX))
            {
                classContentsString.append("<br />+  " + currentSlot->methodSignatureWithoutReturnType());
                ++numLinesOfText;
            }
            else
            {
                //TODOoptional: we definitely don't want to SHOW the name of the unnamed slots (ugly temp hack string), but maybe indicate how many unnamed slots the class has (but don't say "zero" ofc -- when in the class editor, such unnamed slots would be double-clickable and that takes us to the use case they are in)
            }
        }
    }
    return classContentsString;
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::updateClassContentsGraphicsTextItem()
{
    m_ClassContentsGraphicsTextItem->setHtml(classDetailsAsHtmlString());
    setRect(childrenBoundingRect());
}
#if 0 //i believe there's some line drawing logic (in between the member Types) i want to grab out of this before removing altogether
QRectF DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::boundingRect() const
{
    return m_BoundingRect;
    //return QGraphicsWidget::boundingRect();
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->save();
    //painter->setRenderHint(QPainter::Antialiasing, true); //TO DOneoptimization: maybe i can specify this somewhere else so i don't have to keep re-specifying it. also noticed noticeable slowdown when enabled lolol, BUT the level of sexiness hit maximum too. //TODOoptional: run-time option obviously makes a lot of sense
    painter->setBrush(Qt::transparent); //would do white, but i have to draw text first in order to get my bounding rect starting point guh, so a white fill on the rounded rect afterwards covers the words! fml
    painter->setPen(m_ClassBorderPen);
    QString classContentsString(m_DesignEqualsImplementationClass->Name);
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

    //Only draw lines between class sub-Types (signals/slots/private-methods/etc)
    QList<int> indexesInto_i_inForLoopToActuallyDrawLinesFor; //we still need to know the position of all lines for proper spacing
    //int runningIforMarkingLinesToDrawSynchronizedWithLinesDeterminedForSpacing = 0;
    //indexesInto_i_inForLoopToActuallyDrawLinesFor.append(runningIforMarkingLinesToDrawSynchronizedWithLinesDeterminedForSpacing++);
    indexesInto_i_inForLoopToActuallyDrawLinesFor.append(0); //first line after class name
    if(!m_DesignEqualsImplementationClass->properties().isEmpty())
    {
        indexesInto_i_inForLoopToActuallyDrawLinesFor.append(indexesInto_i_inForLoopToActuallyDrawLinesFor.last()+m_DesignEqualsImplementationClass->properties().size());
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
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::handleNonFunctionMemberAdded(NonFunctionMember *propertyAdded)
{
    Q_UNUSED(propertyAdded)
    //TODOreq: re-paint with new property
    //TODOoptmization: when "opening" a file, tons (hundreds, possibly thousands, depending on the project) of these handle* slots will be invoked, each one triggering a repaint. I'm not sure, but actually I think that the calls to update CAN be (and are) combined. If _NOT_, I should probably do that combining myself/hackily!!
    //update(boundingRect()); //TODOoptimization: if our 'thing' (property here) is added at the BOTTOM of the uml/widget, we can supply a smaller rect to update!

    updateClassContentsGraphicsTextItem(); //TODOoptimization: don't update EVERYTHING, just append the new property, ditto for slots/signals/etc
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::handlePrivateMethodAdded(DesignEqualsImplementationClassPrivateMethod *)
{
    //hmm starting to think i should have just emitted the lists themselves, since i'm now thinking i am going to store them in lists also xD
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::handleSlotAdded(DesignEqualsImplementationClassSlot*slotAdded)
{
    //TODOprobably (same as the other handles, so really they can all be merged...):
    //myPaint(); //does draw incl mutex lock
    //update(); //paint works on static item (pixmap?)
    updateClassContentsGraphicsTextItem();
}
void DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene::handleSignalAdded(DesignEqualsImplementationClassSignal *)
{
    updateClassContentsGraphicsTextItem();
}
