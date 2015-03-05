#include "designequalsimplementationchunkofrawcppstatementsgraphicsitemforusecasescene.h"

#include <QPen>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

#include "designequalsimplementationclasslifelineslotgraphicsitemforusecasescene.h"
#include "../../designequalsimplementationproject.h"
#include "../../designequalsimplementationclass.h"
#include "../../designequalsimplementationclassslot.h"
#include "../../designequalsimplementationchunkofrawcppstatements.h"

DesignEqualsImplementationChunkOfRawCppStatementsGraphicsItemForUseCaseScene::DesignEqualsImplementationChunkOfRawCppStatementsGraphicsItemForUseCaseScene(DesignEqualsImplementationChunkOfRawCppStatements *chunkOfRawCppStatements, DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *parentSlotGraphicsItem, QObject *parent)
    : QObject(parent)
    , QGraphicsRectItem(parentSlotGraphicsItem)
    , m_ChunkOfRawCppStatements(chunkOfRawCppStatements)
    , m_ParentSlotGraphicsItem(parentSlotGraphicsItem)
{
    setBrush(Qt::white);
    m_TextGraphicsItem = new QGraphicsTextItem(QObject::tr("C++"), this);
    setRect(childrenBoundingRect());

    connect(this, SIGNAL(editCppModeRequested(DesignEqualsImplementationClass*,DesignEqualsImplementationClassSlot*,int)), parentSlotGraphicsItem->underlyingSlot()->ParentClass->m_ParentProject, SLOT(handleEditCppModeRequested(DesignEqualsImplementationClass*,DesignEqualsImplementationClassSlot*,int)));
}
void DesignEqualsImplementationChunkOfRawCppStatementsGraphicsItemForUseCaseScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *editCppAction = menu.addAction(QObject::tr("Edit C++"));
    QAction *selectedAction = menu.exec(event->screenPos());
    if(!selectedAction)
        return;
    if(selectedAction == editCppAction)
    {
        DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot = m_ParentSlotGraphicsItem->underlyingSlot();
        emit editCppModeRequested(designEqualsImplementationClassSlot->ParentClass, designEqualsImplementationClassSlot, designEqualsImplementationClassSlot->orderedListOfStatements().indexOf(m_ChunkOfRawCppStatements));
        return;
    }
}
