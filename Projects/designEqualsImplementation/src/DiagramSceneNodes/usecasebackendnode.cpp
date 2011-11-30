#include "usecasebackendnode.h"

UseCaseBackEndNode::UseCaseBackEndNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType)
    : DiagramSceneNode(uniqueId, viewType)
{

}
QString UseCaseBackEndNode::getNodeTypeAsString()
{
    return QString("Back-End");
}
