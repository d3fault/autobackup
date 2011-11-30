#include "usecasefrontendnode.h"

UseCaseFrontEndNode::UseCaseFrontEndNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType)
    : DiagramSceneNode(uniqueId, viewType)
{

}
QString UseCaseFrontEndNode::getNodeTypeAsString()
{
    return QString("Front-End");
}
