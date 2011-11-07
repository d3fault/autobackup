#include "designprojectview.h"

//main constructor
void DesignProjectView::privConstructor(QString projectViewName, ProjectViewType projectViewType)
{
    m_ProjectViewName = projectViewName;
    m_ProjectViewType = projectViewType;
}
//helper constructor for new use cases (the primarily used constructor)
DesignProjectView::DesignProjectView(QString useCaseName)
{
     privConstructor(useCaseName, UseCaseType);
}
//helper constructor for class diagrams. only one per project //TODOreq: enforce this at project level
DesignProjectView::DesignProjectView()
{
    privConstructor(QString("Class Diagram"), ClassDiagramType);
}
QString DesignProjectView::getProjectViewName()
{
    return m_ProjectViewName;
}
