#include "diagramscenenode.h"

DiagramSceneNode::DiagramSceneNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType) :
    m_UniqueId(uniqueId), m_ViewType(viewType)
{
}
int DiagramSceneNode::getUniqueId()
{
    return m_UniqueId;
}
DesignProjectTemplates::DesignProjectViewType DiagramSceneNode::getViewType()
{
    return m_ViewType;
}
