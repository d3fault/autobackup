#include "designequalsimplementationchunkofrawcppstatementsgraphicsitemforusecasescene.h"

#include <QPen>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

#include "designequalsimplementationclasslifelineslotgraphicsitemforusecasescene.h"

DesignEqualsImplementationChunkOfRawCppStatementsGraphicsItemForUseCaseScene::DesignEqualsImplementationChunkOfRawCppStatementsGraphicsItemForUseCaseScene(DesignEqualsImplementationChunkOfRawCppStatements *chunkOfRawCppStatements, DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *parentSlotGraphicsItem)
    : QGraphicsRectItem(parentSlotGraphicsItem)
    , m_ChunkOfRawCppStatements(chunkOfRawCppStatements)
    , m_ParentSlotGraphicsItem(parentSlotGraphicsItem)
{
    setBrush(Qt::white);
    m_TextGraphicsItem = new QGraphicsTextItem(QObject::tr("C++"), this);
    setRect(childrenBoundingRect());
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
        //emit cppEditModeRequested();
        return;
    }
}
