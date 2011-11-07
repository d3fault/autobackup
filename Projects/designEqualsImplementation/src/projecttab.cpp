#include "projecttab.h"

ProjectTab::ProjectTab(DesignProject *project) : m_Failed(true)
{
    //save arguments
    m_Project = project;

    //gui
    m_Layout = new QVBoxLayout();
    m_ProjectViewsTabContainer = new QTabWidget();
    m_Layout->addWidget(m_ProjectViewsTabContainer);
    this->setLayout(m_Layout);

    //connections
    connect(m_ProjectViewsTabContainer, SIGNAL(currentChanged(int)), this, SLOT(handleProjectViewsTabChanged(int)));
}
DesignProject * ProjectTab::getProject()
{
    return m_Project;
}
void ProjectTab::handleProjectViewsTabChanged(int newIndex)
{
    m_CurrentProjectView = qobject_cast<ProjectViewTab*>(m_ProjectViewsTabContainer->widget(newIndex))->getProjectView();
    if(!m_CurrentProjectView)
    {
        emit e("Project View Cast Failed");
        m_Failed = true;
        return;
    }
    m_Failed = false;

    //after this, nothing really... maybe redraw the qgraphicsscene based on the contents of the [new] current project view...
    //...but mainly this is only necessary for later drag and drops / oonnections made
}
void ProjectTab::createEmptyClassDiagram()
{
    DesignProjectView *emptyClassDiagram = new DesignProjectView();
    ProjectViewTab *classDiagramViewTab = new ProjectViewTab(emptyClassDiagram);
    connect(classDiagramViewTab, SIGNAL(e(const QString &)), this, SIGNAL(e(const QString &)));
    int newTabIndex = m_ProjectViewsTabContainer->addTab(classDiagramViewTab, emptyClassDiagram->getProjectViewName());
    m_ProjectViewsTabContainer->widget(newTabIndex);
}
