#include "designequalsimplementationchunkofrawcppstatementsgraphicsitemforusecasescene.h"

#include <QPen>

DesignEqualsImplementationChunkOfRawCppStatementsGraphicsItemForUseCaseScene::DesignEqualsImplementationChunkOfRawCppStatementsGraphicsItemForUseCaseScene(DesignEqualsImplementationChunkOfRawCppStatements *chunkOfRawCppStatements, QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
    , m_ChunkOfRawCppStatements(chunkOfRawCppStatements)
{
    setBrush(Qt::white);
    m_TextGraphicsItem = new QGraphicsTextItem(QObject::tr("C++"), this);
    setRect(childrenBoundingRect());
}
