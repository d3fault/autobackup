#include "classdiagramfrontendnode.h"

ClassDiagramFrontEndNode::ClassDiagramFrontEndNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType)
    : DiagramSceneNode(uniqueId, viewType)
{
    drawMyPolygon();
    setPolygon(m_MyPolygon);
}
QString ClassDiagramFrontEndNode::getNodeTypeAsString()
{
    return QString("Front-End");
}
void ClassDiagramFrontEndNode::drawMyPolygon()
{
    m_MyPolygon << QPointF(-100, 0) << QPointF(0, 100)
                          << QPointF(100, 0) << QPointF(0, -100)
                          << QPointF(-100, 0);
}
#if 0
DiagramSceneNode *ClassDiagramFrontEndNode::cloneSelf()
{
    DiagramSceneNode *clone = new ClassDiagramFrontEndNode(m_UniqueId, m_ViewType);
    return clone;
}
#endif
