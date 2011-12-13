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
    connect(m_Project, SIGNAL(projectViewAdded(DesignProjectView*)), this, SLOT(handleProjectViewAdded(DesignProjectView*)));
}
DesignProject * ProjectTab::getProject()
{
    return m_Project;
}
void ProjectTab::handleProjectViewsTabChanged(int newIndex)
{
    DesignProjectView *projectViewOnNewTab = qobject_cast<ProjectViewTab*>(m_ProjectViewsTabContainer->widget(newIndex))->getProjectView();
    if(!projectViewOnNewTab)
    {
        emit e("Project View Cast Failed");
        m_Failed = true;
        return;
    }
    m_Failed = false;

    ProjectController::Instance()->setCurrentProject(m_Project); //hack, but seems to be the best spot idk...
    ProjectController::Instance()->getCurrentProject()->setCurrentProjectView(projectViewOnNewTab);

    //after this, nothing really... maybe redraw the qgraphicsscene based on the contents of the [new] current project view...
    //...but mainly this is only necessary for later drag and drops / connections made
}
void ProjectTab::handleProjectViewAdded(DesignProjectView *newProjectView)
{
    //create a project view tab for the new project view
    ProjectViewTab *classDiagramViewTab = new ProjectViewTab(newProjectView);
    connect(classDiagramViewTab, SIGNAL(e(const QString &)), this, SIGNAL(e(const QString &)));
    int newTabIndex = m_ProjectViewsTabContainer->addTab(classDiagramViewTab, newProjectView->getProjectViewName());
    m_ProjectViewsTabContainer->setCurrentIndex(newTabIndex);
}
