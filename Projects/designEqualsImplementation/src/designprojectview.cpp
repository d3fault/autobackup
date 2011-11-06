#include "designprojectview.h"

//main constructor
DesignProjectView::DesignProjectView(QString projectViewName, ProjectViewType projectViewType) :
    m_ProjectViewName(projectViewName), m_ProjectViewType(projectViewType)
{

}
//helper constructor for new use cases (the primarily used constructor)
DesignProjectView::DesignProjectView(QString useCaseName)
{
     DesignProjectView(useCaseName, UseCaseType);
}
//helper constructor for class diagrams. only one per project //TODOreq: enforce this at project level
DesignProjectView::DesignProjectView()
{
    DesignProjectView(tr("Class Diagram"), ClassDiagramType);
}
