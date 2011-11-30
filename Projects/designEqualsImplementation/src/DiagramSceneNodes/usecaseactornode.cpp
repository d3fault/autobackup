#include "usecaseactornode.h"

UseCaseActorNode::UseCaseActorNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType)
    : DiagramSceneNode(uniqueId, viewType)
{

}
QString UseCaseActorNode::getNodeTypeAsString()
{
    return QString("Actor");
}
