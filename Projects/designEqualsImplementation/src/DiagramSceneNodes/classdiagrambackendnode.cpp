#include "classdiagrambackendnode.h"

ClassDiagramBackEndNode::ClassDiagramBackEndNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType)
    : DiagramSceneNode(uniqueId, viewType)
{

}
QString ClassDiagramBackEndNode::getNodeTypeAsString()
{
    return QString("Back-End");
}
