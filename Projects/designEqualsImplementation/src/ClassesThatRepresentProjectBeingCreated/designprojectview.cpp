#include "designprojectview.h"

//main constructor
void DesignProjectView::privConstructor(QString projectViewName, DesignProjectTemplates::DesignProjectViewType projectViewType)
{
    m_ProjectViewName = projectViewName;
    m_ProjectViewType = projectViewType;
    m_ListOfNodes = new QMap<DiagramSceneNode*,QPointF>();
}
//helper constructor for new use cases (the primarily used constructor)
DesignProjectView::DesignProjectView(QString useCaseName)
{
     privConstructor(useCaseName, DesignProjectTemplates::UseCaseViewType);
     m_ThisViewIsClassDiagram = false;
}
//helper constructor for class diagrams. only one per project //TODOreq: enforce this at project level
DesignProjectView::DesignProjectView()
{
    privConstructor(QString("Class Diagram"), DesignProjectTemplates::ClassDiagramViewType);
    m_ThisViewIsClassDiagram = true;
}
QString DesignProjectView::getProjectViewName()
{
    return m_ProjectViewName;
}
void DesignProjectView::addPendingNodeAt(QPointF pointNodeIsAtInScene)
{
    DiagramSceneNode *nodeToAdd = ProjectController::Instance()->getPendingNode();

    DiagramSceneNode *nodeToAddNewInstance = (DiagramSceneNode*)qMalloc(sizeof(DiagramSceneNode));
    qMemCopy(nodeToAddNewInstance,nodeToAdd,sizeof(DiagramSceneNode)); //lol hacky as fuck idk if this'll work
    //DiagramSceneNode *nodeToAddNewInstance = nodeToAdd->copyNode();

    m_ListOfNodes->insert(nodeToAddNewInstance,pointNodeIsAtInScene);
    nodeToAddNewInstance->setPos(pointNodeIsAtInScene);
    emit nodeAdded(nodeToAddNewInstance);
}

//TODOreq: i need to expand this class so that it contains a parcelable list of nodes. additionally, the parcelable list contains the geographic coordinates in the design scene... but this is not useful information that the GENERATOR needs
