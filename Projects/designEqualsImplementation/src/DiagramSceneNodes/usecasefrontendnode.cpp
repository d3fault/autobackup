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
#if 0
DiagramSceneNode *UseCaseFrontEndNode::cloneSelf()
{
    DiagramSceneNode *clone = new UseCaseFrontEndNode(m_UniqueId, m_ViewType);
    return clone;
}
#endif
