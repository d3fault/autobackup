#include "openprojectsmanager.h"

OpenProjectsManager::OpenProjectsManager(QObject *parent) :
    QObject(parent)
{
}
void OpenProjectsManager::add(DesignProject *designProject)
{
    m_ListOfOpenProjects->append(designProject);
    emit projectOpened(designProject);
}
DesignProject * OpenProjectsManager::getByTabIndex(int tabIndex)
{
}
//TODOreq: close(DesignProject*)
