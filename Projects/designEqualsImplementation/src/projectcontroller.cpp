#include "projectcontroller.h"

#include "ClassesThatRepresentProjectBeingCreated/designproject.h"
#include "Gui/diagramscenenode.h"

ProjectController::ProjectController()
{

}

ProjectController* ProjectController::m_pInstance = NULL;
ProjectController* ProjectController::Instance()
{
    if(!m_pInstance) //only one instance allowed
    {
        m_pInstance = new ProjectController();
    }
    return m_pInstance;
}
DesignProject * ProjectController::getCurrentProject()
{
    return m_CurrentProject;
}
void ProjectController::setCurrentProject(DesignProject *newCurrentProject)
{
    m_CurrentProject = newCurrentProject;
    emit currentProjectChanged(newCurrentProject);
}
void ProjectController::setPendingNode(DiagramSceneNode *node)
{
    m_PendingNode = node;
}
DiagramSceneNode * ProjectController::getPendingNode()
{
    return m_PendingNode;
}
