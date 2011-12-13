#include "classdiagrambackendnode.h"

ClassDiagramBackEndNode::ClassDiagramBackEndNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType)
    : DiagramSceneNode(uniqueId, viewType)
{
    drawMyPolygon();
    setPolygon(m_MyPolygon);
}
QString ClassDiagramBackEndNode::getNodeTypeAsString()
{
    return QString("Back-End");
}
void ClassDiagramBackEndNode::drawMyPolygon()
{
    //m_MyPolygon << QPointF(-100, 0) << QPointF(0, 100) << QPointF(100, 0) << QPointF(0, -100) << QPointF(-100, 0);
    m_MyPolygon << QPointF(-100, -100) << QPointF(100, -100) << QPointF(100, 100) << QPointF(-100, 100) << QPointF(-100, -100);
}
DesignProjectTemplates::DesignProjectViewNodeType ClassDiagramBackEndNode::getNodeType()
{
    return DesignProjectTemplates::ClassDiagramBackEndNodeType;
}
