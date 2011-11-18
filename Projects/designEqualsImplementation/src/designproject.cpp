#include "designproject.h"

DesignProject::DesignProject(QString projectName) :
    m_ProjectName(projectName)
{
    m_ProjectViewList = new QList<DesignProjectView*>();
}
void DesignProject::addProjectView(DesignProjectView *designProjectView)
{
    m_ProjectViewList->append(designProjectView);
    emit projectViewAdded(designProjectView);
}
void DesignProject::createEmptyProject()
{
    //for now, just create 1 empty class diagram design view. the signal will tell ProjectTab to add a new tab for the class diagram
    DesignProjectView *emptyClassDiagram = new DesignProjectView();
    this->addProjectView(emptyClassDiagram);
}
