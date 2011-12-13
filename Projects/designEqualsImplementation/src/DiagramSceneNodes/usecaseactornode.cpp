#include "usecaseactornode.h"

UseCaseActorNode::UseCaseActorNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType)
    : DiagramSceneNode(uniqueId, viewType)
{
    drawMyPolygon();
    setPolygon(m_MyPolygon);
}
QString UseCaseActorNode::getNodeTypeAsString()
{
    return QString("Actor");
}
void UseCaseActorNode::drawMyPolygon()
{
    m_MyPolygon << QPointF(-100, 0) << QPointF(0, 100)
                          << QPointF(100, 0) << QPointF(0, -100)
                          << QPointF(-100, 0);
}
DesignProjectTemplates::DesignProjectViewNodeType UseCaseActorNode::getNodeType()
{
    return DesignProjectTemplates::UseCaseActorNodeType;
}
