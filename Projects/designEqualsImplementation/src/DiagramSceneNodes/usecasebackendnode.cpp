#include "usecasebackendnode.h"

UseCaseBackEndNode::UseCaseBackEndNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType)
    : DiagramSceneNode(uniqueId, viewType)
{
    drawMyPolygon();
    setPolygon(m_MyPolygon);
}
QString UseCaseBackEndNode::getNodeTypeAsString()
{
    return QString("Back-End");
}
void UseCaseBackEndNode::drawMyPolygon()
{
    m_MyPolygon << QPointF(-100, 0) << QPointF(0, 100)
                          << QPointF(100, 0) << QPointF(0, -100)
                          << QPointF(-100, 0);
}
DesignProjectTemplates::DesignProjectViewNodeType UseCaseBackEndNode::getNodeType()
{
    return DesignProjectTemplates::UseCaseBackEndNodeType;
}
