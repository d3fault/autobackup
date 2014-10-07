#include "designequalsimplementationclasslifelinegraphicsitemforusecasescene.h"

#include <QPainter>
#include <QLineF>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QListIterator>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>

#include "designequalsimplementationguicommon.h"
#include "designequalsimplementationclasslifelineslotgraphicsitemforusecasescene.h"
#include "classinstancechooserdialog.h"
#include "classeditordialog.h"
#include "../../designequalsimplementationclass.h"
#include "../../designequalsimplementationusecase.h"
#include "../../designequalsimplementationclasslifeline.h"

#define DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_NO_ACTIVITY_LIFELINE_MINIMUM_VERTICAL_GAP 15

//TODOreq: if a slot invoked on another thread (to the right) invokes us (via signal or whatever) by drawing an arrow back to that same unit of execution, a new unit of execution is auto-created right then and there for it
//TODOmb: right-click -> new unit of execution (would be nameless, just like the very first one added)
//TODOmb: as soon as a slot is named, the next "dummy fake temp slot" should be created. all snap points (destination snap points) for the just-named slot should be redirected to the newly created "dummy fake temp slot"... but there should still be a visual cue of this redirecting in case that "dummy fake temp slot" is off the screen (perhaps the one they are aiming at has tons of statements on it, making the "dummy fake temp slot" be down below the fold. this is more or less already being done (when they release the mouse button, the new slot is created)... but the snap point re-routing logic when the mouse is still being held down is not being done
DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(UseCaseGraphicsScene *parentUseCaseGraphicsScene, DesignEqualsImplementationClassLifeLine *classLifeLine, QObject *qobjectParent, QGraphicsItem *graphicsItemParent)
    : QObject(qobjectParent)
    , QGraphicsRectItem(graphicsItemParent)
    , m_ParentUseCaseGraphicsScene(parentUseCaseGraphicsScene)
{
    privateConstructor(classLifeLine);
}
DesignEqualsImplementationClassLifeLine *DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::classLifeLine() const
{
    return m_DesignEqualsImplementationClassLifeLine;
}
int DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::type() const
{
    return DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ClassLifeLine_GRAPHICS_TYPE_ID;
}
//TODOreq: right-clicking a slot graphics item also brings up context menu
void DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *chooseInstanceAction = menu.addAction(tr("Choose Instance"));
    QAction *classEditorAction = menu.addAction(tr("Class Editor"));
    QAction *removeClassLifelineFromUseCaseAction = menu.addAction(tr("Remove Class Lifeline from Use Case"));
    QAction *selectedAction = menu.exec(event->screenPos());
    if(!selectedAction)
        return;
    if(selectedAction == chooseInstanceAction)
    {
        ClassInstanceChooserDialog classInstanceChooserDialog(classLifeLine());
        classInstanceChooserDialog.exec();
        return;
    }
    else if(selectedAction == classEditorAction)
    {
        ClassEditorDialog classEditorDialog(classLifeLine()->designEqualsImplementationClass(), m_DesignEqualsImplementationClassLifeLine->parentUseCase()->designEqualsImplementationProject());
        if(classEditorDialog.exec() != QDialog::Accepted)
            return;
    }
    else if(selectedAction == removeClassLifelineFromUseCaseAction) //etc
    {
        //TODOreq
    }
}
UseCaseGraphicsScene *DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::parentUseCaseGraphicsScene() const
{
    return m_ParentUseCaseGraphicsScene;
}
void DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::maybeMoveLeftOrRightBecauseNewSlotInvokeStatementWasConnected()
{
    //TODOreq: find max slot width and then set our pos based on it
    qreal width = 10; //TODoreq: better default? probably won't be used anyways?
    Q_FOREACH(DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *currentSlotGraphicsItem, m_SlotGraphicsItemsInThisClassLifeLine)
    {
        //TODOreq: we are only interested in slot invokes from the class lifeline directly to the left of this class lifeline (right?), wtf?
        //TODOreq: class lifelines in between the slot invoker and slot invokee are accounted for when calculating the "max width" thingo
        //TODOreq: we don't necessarily have ANY slot invocations from the class lifeline directly to the left of this class lifeline, but we still need to calculate a good width
        //TODOreq: width = qMax(width, currentSlotGraphicsItem->invokingStatementGraphicsItem()->boundingRect().width());
    }

}
DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::slotGraphicsItemForSlot_OrZeroIfNotFound(DesignEqualsImplementationClassSlot *theSlot)
{
    Q_FOREACH(DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *currentSlotGraphicsItem, m_SlotGraphicsItemsInThisClassLifeLine)
    {
        if(currentSlotGraphicsItem->underlyingSlot() == theSlot)
            return currentSlotGraphicsItem;
    }
    return 0;
}
void DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::privateConstructor(DesignEqualsImplementationClassLifeLine *classLifeLine)
{
    m_DesignEqualsImplementationClassLifeLine = classLifeLine;
    setFlag(QGraphicsItem::ItemIsMovable, true);
    //setFlag(QGraphicsItem::ItemIsSelectable, true);
    m_ClassBorderPen.setWidth(2);

    QString lifeLineTitleHtml("<b>");
#if 0 //TODOinstancing
    if(classLifeLine->myInstanceInClassThatHasMe()->m_InstanceType.first)
    {
        lifeLineTitleHtml += classLifeLine->myInstanceInClassThatHasMe()->m_MyClass->ClassName + "</b>" + "* " /*exception to the rule, I want the star to be close to the type and not the class that owns me*/ + classLifeLine->myInstanceInClassThatHasMe()->m_InstanceType.first->ClassName + "::" + classLifeLine->myInstanceInClassThatHasMe()->VariableName;
    }
    else
    {
        lifeLineTitleHtml += classLifeLine->designEqualsImplementationClass()->ClassName + "</b>";
    }
#else
    lifeLineTitleHtml += classLifeLine->designEqualsImplementationClass()->ClassName + "</b>";
#endif

    QGraphicsTextItem *classNameTextItem = new QGraphicsTextItem(this);
    classNameTextItem->setHtml(lifeLineTitleHtml);

    QRectF myRect = childrenBoundingRect();
    //myRect.moveLeft(-myRect.width()/2);
    //myRect.moveTop(-myRect.height()/2);
    //myRect.moveBottom(0);
    //myRect.moveCenter(QPointF(0, 0));
    setRect(myRect);

    //classNameTextItem->setPos(rect().top(), rect().left());
    //classNameTextItem->setPos();

    int currentIndex = 0;
    QListIterator<DesignEqualsImplementationClassSlot*> slotsReferenceCountIterator(classLifeLine->mySlotsAppearingInClassLifeLine());
    while(slotsReferenceCountIterator.hasNext())
    {
        createAndInsertSlotGraphicsItem(currentIndex++, slotsReferenceCountIterator.next());
        //new DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene(currentUnitOfExecution, this, unitOfExecutionRect, this);
    }
    repositionSlotsBecauseOneSlotsChanged();
    connect(classLifeLine, SIGNAL(slotInsertedIntoClassLifeLine(int,DesignEqualsImplementationClassSlot*)), this, SLOT(handleSlotInsertedIntoClassLifeLine(int, DesignEqualsImplementationClassSlot*)));
    connect(classLifeLine, SIGNAL(slotRemovedFromClassLifeLine(DesignEqualsImplementationClassSlot*)), this, SLOT(handleSlotRemovedFromClassLifeLine(DesignEqualsImplementationClassSlot*)));
}
DesignEqualsImplementationSlotGraphicsItemForUseCaseScene* DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::createAndInsertSlotGraphicsItem(int indexInsertedInto, DesignEqualsImplementationClassSlot *slot)
{
    DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *slotGraphicsItem = new DesignEqualsImplementationSlotGraphicsItemForUseCaseScene(this, slot, this);
    m_SlotGraphicsItemsInThisClassLifeLine.insert(indexInsertedInto, slotGraphicsItem);
    connect(slotGraphicsItem, SIGNAL(geometryChanged()), this, SLOT(handleSlotGeometryChanged()));
    return slotGraphicsItem;
}
//TODOreq: in the future the "lines in between" lengths should be user stretchable and serializable, but for now to KISS they'll be fixed length (primarily, the unit of execution rect itself will auto-move (which will indirectly lengthen the line above it) or stretch, but it does make sense for the user to put it right where they want it vertically
//TODOreq: unit of executions changing (insert/remove) ultimately triggers repositionUnitsOfExecution, because their positions depend on their sizes
void DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::repositionSlotsBecauseOneSlotsChanged() //TODOoptimization: an append would be much easier/faster, but I should get the essentials out of the way first
{
    //TODOreq: we don't control the unit of execution's SIZE, but we still want to position it based on it's rect's top y value.. but we can't just simply moveTop because that would mess up the dynamic sizing stuff within

    QPen dashedPen;
    dashedPen.setStyle(Qt::DotLine);
    while(m_DottedLinesJustAboveEachSlot.size() < m_SlotGraphicsItemsInThisClassLifeLine.size())
    {
        QGraphicsLineItem *newLineItem = new QGraphicsLineItem(this);
        newLineItem->setLine(0, 0, 0, DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_NO_ACTIVITY_LIFELINE_MINIMUM_VERTICAL_GAP);
        m_DottedLinesJustAboveEachSlot.append(newLineItem);
        newLineItem->setPen(dashedPen);
    }

#if 0
    QPointF topOfLifeLine, bottomOfLifeLine;
    topOfLifeLine.setX((rect().right()-rect().left())/2);
    bottomOfLifeLine.setX(topOfLifeLine.x());
    topOfLifeLine.setY(rect().bottom());
    bottomOfLifeLine.setY(topOfLifeLine.y()+DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_NO_ACTIVITY_LIFELINE_MINIMUM_VERTICAL_GAP);
    QLineF lineAboveUnitOfExecution(topOfLifeLine, bottomOfLifeLine);
#endif
    QPointF currentTopMiddlePointOfDottedLineAboveUnitOfExecution;
    currentTopMiddlePointOfDottedLineAboveUnitOfExecution.setX(rect().width()/2);
    currentTopMiddlePointOfDottedLineAboveUnitOfExecution.setY(rect().bottom());
    int currentLineIndex = 0;
    Q_FOREACH(DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *currentSlotGraphicsItem, m_SlotGraphicsItemsInThisClassLifeLine)
    {
        //m_DottedLinesJustAboveEachUnitOfExecution.at(currentLineIndex++)->setLine(lineAboveUnitOfExecution); //TODOreq: drawing arrow to these dashed lines should behave just like drawing arrow to the class name box
        //QPointF dottedLinePos = bottomMiddleOfClassNameRect;
        //qreal dottedLineHalfHeight = (m_DottedLinesJustAboveEachUnitOfExecution.at(currentLineIndex)->line().length()/2);
        //dottedLinePos.setY(dottedLinePos.y() + dottedLineHalfHeight);
        QGraphicsItem *currentLineItem = m_DottedLinesJustAboveEachSlot.at(currentLineIndex++);
        currentLineItem->setPos(currentTopMiddlePointOfDottedLineAboveUnitOfExecution);

        /*QPointF topLeftOfUnitOfExecutionRect(bottomOfLifeLine.x()-DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_HALF_WIDTH, bottomOfLifeLine.y());
        QPointF bottomRightOfUnitOfExecutionRect(bottomOfLifeLine.x()+DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_HALF_WIDTH, bottomOfLifeLine.y()+DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_UNIT_OF_EXECUTION_MINIMUM_VERTICAL_SIZE);
        QRectF unitOfExecutionRect(topLeftOfUnitOfExecutionRect, bottomRightOfUnitOfExecutionRect);*/

        //currentUnitOfExecutionGraphicsItem->setRect(unitOfExecutionRect); //or don't I mean setPos, since I am the parent and he manages his size?
        //QPointF topMiddlePos(rect().width()/2, bottomOfLifeLine.y());
        QPointF centerPointOfUnitOfExecution = currentTopMiddlePointOfDottedLineAboveUnitOfExecution;
        centerPointOfUnitOfExecution.setY(centerPointOfUnitOfExecution.y() + currentLineItem->boundingRect().height());
        qreal halfUnitOfExecutionHeight = (currentSlotGraphicsItem->boundingRect().height()/2);
        centerPointOfUnitOfExecution.setY(centerPointOfUnitOfExecution.y() + halfUnitOfExecutionHeight);
        //bottomOfDottedLine.setY(bottomOfDottedLine.y() + dottedLineHalfHeight);
        currentSlotGraphicsItem->setPos(centerPointOfUnitOfExecution);


        //calcuate next dotted line topMiddle position, even if there aren't any more slots (which means there aren't any more dotted lines)
        currentTopMiddlePointOfDottedLineAboveUnitOfExecution.setY(centerPointOfUnitOfExecution.y() + halfUnitOfExecutionHeight);
#if 0
        topOfLifeLine.setY(unitOfExecutionRect.bottom());
        bottomOfLifeLine.setY(topOfLifeLine.y()+DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene_NO_ACTIVITY_LIFELINE_MINIMUM_VERTICAL_GAP);
        lineAboveUnitOfExecution.setPoints(topOfLifeLine, bottomOfLifeLine);
#endif
    }
}
void DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::handleSlotInsertedIntoClassLifeLine(int indexInsertedInto, DesignEqualsImplementationClassSlot *slot)
{
    //make the graphics item, add it as child, and reposition
    DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *slotGraphicsItem = createAndInsertSlotGraphicsItem(indexInsertedInto, slot);
    repositionSlotsBecauseOneSlotsChanged();

    //emit signal that use case graphics scene is listening to. he updates arrow position if exists
    emit slotGraphicsItemInsertedIntoClassLifeLineGraphicsItem(slotGraphicsItem);
}
void DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::handleSlotRemovedFromClassLifeLine(DesignEqualsImplementationClassSlot *slotRemoved)
{
    QMutableListIterator<DesignEqualsImplementationSlotGraphicsItemForUseCaseScene*> slotGraphicsItemIterator(m_SlotGraphicsItemsInThisClassLifeLine);
    while(slotGraphicsItemIterator.hasNext())
    {
        DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *slotGraphicsItem = slotGraphicsItemIterator.next();
        if(slotGraphicsItem->underlyingSlot() == slotRemoved)
        {
            slotGraphicsItemIterator.remove();
            delete slotGraphicsItem;
        }
    }
}
void DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::handleSlotGeometryChanged()
{
    repositionSlotsBecauseOneSlotsChanged();
}
#if 0
QRectF DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::boundingRect() const
{
    return m_BoundingRect;
}
void DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->save();
    painter->setBrush(Qt::transparent);

    //draw class name and surrounding box
    painter->setPen(m_ClassBorderPen);
    QRectF resizeBoundingRectTo;
    painter->drawText(boundingRect(), (Qt::AlignVCenter /*| Qt::TextDontClip*/), m_DesignEqualsImplementationClassLifeLine->designEqualsImplementationClass()->ClassName, &resizeBoundingRectTo); //TODOreq: unsafe pointer usage :)
    QRectF roundedRect = resizeBoundingRectTo;
    roundedRect.setLeft(roundedRect.left()-(DESIGNEQUALSIMPLEMENTATION_GUI_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT));
    roundedRect.setTop(roundedRect.top()-(DESIGNEQUALSIMPLEMENTATION_GUI_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT));
    roundedRect.setWidth(roundedRect.width()+(DESIGNEQUALSIMPLEMENTATION_GUI_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT));
    roundedRect.setHeight(roundedRect.height()+(DESIGNEQUALSIMPLEMENTATION_GUI_SPACING_FROM_CLASS_TEXT_TO_ROUNDED_RECT));
    painter->drawRoundedRect(roundedRect, DESIGNEQUALSIMPLEMENTATION_GUI_CLASS_GRAPHICS_ITEM_ROUNDED_RECTANGLE_RADIUS, DESIGNEQUALSIMPLEMENTATION_GUI_CLASS_GRAPHICS_ITEM_ROUNDED_RECTANGLE_RADIUS);

#if 0
    //draw [first] no activity life line
    painter->setPen(m_LifelineNoActivityPen);

    painter->drawLine(topLifeLine, bottomOfLifeLine);
#endif

    //(TODOreq: a lifeline with no use should be able to be auto-trimmed from all use cases in project (like inkscape's "vacuum defs"))

#if 0
    //draw [first] unit of execution rectangle

    //painter->drawRect()
    painter->drawRect(unitOfExecutionRect);
#endif

    //bleh I think I need aggregate items in order to [easily ('intersects')] see which unit of execution the arrow being drawn comes from... and i probably need to fix my bounding rect stupidity to support that

    m_BoundingRect = resizeBoundingRectTo;
    painter->restore();
}
#endif
