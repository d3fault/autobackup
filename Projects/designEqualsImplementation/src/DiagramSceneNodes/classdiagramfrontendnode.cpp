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
DesignProjectTemplates::DesignProjectViewNodeType ClassDiagramFrontEndNode::getNodeType()
{
    return DesignProjectTemplates::ClassDiagramFrontEndNodeType;
}
