#include "modesingleton.h"

//broke this mode out into it's own class so i can keep it separate from both mainwindow and scene
//i think i can still have multiple scene placement (project -> project) this way, not sure though

ModeSingleton::ModeSingleton() :
    m_Mode(ClickDragDefaultMode)
{
}
ModeSingleton* ModeSingleton::m_pInstance = NULL;
ModeSingleton * ModeSingleton::Instance()
{
    if(!m_pInstance) //only one instance allowed
    {
        m_pInstance = new ModeSingleton();
    }
    return m_pInstance;
}
void ModeSingleton::setMode(ModeSingleton::Mode newMode)
{
    m_Mode = newMode;
    emit modeChanged(newMode);
}
void ModeSingleton::setPendingNodeToAdd(DiagramSceneNode *pendingNode)
{
    m_PendingNode = pendingNode;
}
DiagramSceneNode * ModeSingleton::getPendingNode()
{
    return m_PendingNode;
}
