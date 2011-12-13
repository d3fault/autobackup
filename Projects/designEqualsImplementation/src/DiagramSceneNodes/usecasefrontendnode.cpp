#include "usecasefrontendnode.h"

UseCaseFrontEndNode::UseCaseFrontEndNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType)
    : DiagramSceneNode(uniqueId, viewType)
{
    drawMyPolygon();
    setPolygon(m_MyPolygon);
}
QString UseCaseFrontEndNode::getNodeTypeAsString()
{
    return QString("Front-End");
}
void UseCaseFrontEndNode::drawMyPolygon()
{
    m_MyPolygon << QPointF(-100, 0) << QPointF(0, 100)
                          << QPointF(100, 0) << QPointF(0, -100)
                          << QPointF(-100, 0);
}
DesignProjectTemplates::DesignProjectViewNodeType UseCaseFrontEndNode::getNodeType()
{
    return DesignProjectTemplates::UseCaseFrontEndNodeType;
}
