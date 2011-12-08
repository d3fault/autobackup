#include "diagramscenenode.h"

DiagramSceneNode::DiagramSceneNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType) :
    m_UniqueId(uniqueId), m_ViewType(viewType)
{
}
int DiagramSceneNode::getUniqueId()
{
    return m_UniqueId;
}
DesignProjectTemplates::DesignProjectViewType DiagramSceneNode::getViewType()
{
    return m_ViewType;
}
QPixmap DiagramSceneNode::image() const
{
    QPixmap pixmap(250, 250);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));
    painter.translate(125, 125);
    painter.drawPolyline(m_MyPolygon);
    return pixmap;
}
