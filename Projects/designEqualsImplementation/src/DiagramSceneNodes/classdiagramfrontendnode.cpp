#include "classdiagramfrontendnode.h"

ClassDiagramFrontEndNode::ClassDiagramFrontEndNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType)
    : DiagramSceneNode(uniqueId, viewType)
{

}
QString ClassDiagramFrontEndNode::getNodeTypeAsString()
{
    return QString("Front-End");
}
