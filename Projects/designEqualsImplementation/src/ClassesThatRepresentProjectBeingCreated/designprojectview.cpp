#include "designprojectview.h"

//main constructor
void DesignProjectView::privConstructor(QString projectViewName, DesignProjectTemplates::DesignProjectViewType projectViewType)
{
    m_ProjectViewName = projectViewName;
    m_ProjectViewType = projectViewType;
}
//helper constructor for new use cases (the primarily used constructor)
DesignProjectView::DesignProjectView(QString useCaseName)
{
     privConstructor(useCaseName, DesignProjectTemplates::UseCaseViewType);
}
//helper constructor for class diagrams. only one per project //TODOreq: enforce this at project level
DesignProjectView::DesignProjectView()
{
    privConstructor(QString("Class Diagram"), DesignProjectTemplates::ClassDiagramViewType);
}
QString DesignProjectView::getProjectViewName()
{
    return m_ProjectViewName;
}

//TODOreq: i need to expand this class so that it contains a parcelable list of nodes. additionally, the parcelable list contains the geographic coordinates in the design scene... but this is not useful information that the GENERATOR needs
