#include "designprojecttemplates.h"

#include "../Gui/diagramscenenode.h"

#include "../DiagramSceneNodes/classdiagramfrontendnode.h"
#include "../DiagramSceneNodes/classdiagrambackendnode.h"

#include "../DiagramSceneNodes/usecaseactornode.h"
#include "../DiagramSceneNodes/usecasefrontendnode.h"
#include "../DiagramSceneNodes/usecasebackendnode.h"

DesignProjectTemplates* DesignProjectTemplates::m_pInstance = NULL;
DesignProjectTemplates* DesignProjectTemplates::Instance()
{
    if(!m_pInstance) //only one instance allowed
    {
        m_pInstance = new DesignProjectTemplates();
    }
    return m_pInstance;
}
//action1() etc

DesignProjectTemplates::DesignProjectTemplates()
{
    //i'm a bit confused on how to do this design. i need to declare each of the underlying types as an object... but there is no reference to them EXCEPT when they are added to the list below
    //that's how the rest of the app finds out the project view types (class diagram, use case), and what diagramscenenodes that project view type supports (actor, frontend, backend, etc)

    //TODOopt: another class diagram scene node type can be USE CASE ENTRANCE. it puts a tiny actor next to where the use case first calls the front-end (use cases don't have to call a front-end.. but a user/actor DOES)
    //so maybe not a tiny actor... but a tiny "UC" or something

    m_AllDesignProjectNodesByUniqueId = new QMap<int, DiagramSceneNode*>();
    m_AllDesignProjectNodesByProjectViewType = new QMultiMap<DesignProjectViewType, DiagramSceneNode*>();
    //populateDesignProjectViewsAndTheirNodes();
}
void DesignProjectTemplates::populateDesignProjectViewsAndTheirNodes()
{
    int i = 0;

    //the order in which these are added to the qmap defines the order they show up in the gui

    //class diagram
    m_AllDesignProjectNodesByUniqueId->insert(i, new ClassDiagramFrontEndNode(i, ClassDiagramViewType));
    ++i;
    m_AllDesignProjectNodesByUniqueId->insert(i, new ClassDiagramBackEndNode(i, ClassDiagramViewType));
    ++i;

    //use case
    m_AllDesignProjectNodesByUniqueId->insert(i, new UseCaseActorNode(i, UseCaseViewType));
    ++i;
    m_AllDesignProjectNodesByUniqueId->insert(i, new UseCaseFrontEndNode(i, UseCaseViewType));
    ++i;
    m_AllDesignProjectNodesByUniqueId->insert(i, new UseCaseBackEndNode(i, UseCaseViewType));
    ++i;

    //TODOopt: the below could be generated from the above map on the fly when requested, but we'll leave it out of laziness
    //might be smarter to leave it being generated here as this way we only generate it once if it's requested more than once. idk or care. premature opt...
    for(int j = 0; j < i; ++j)
    {
        DiagramSceneNode *currentDiagramSceneNode = m_AllDesignProjectNodesByUniqueId->value(j);
        m_AllDesignProjectNodesByProjectViewType->insert(currentDiagramSceneNode->getViewType(), currentDiagramSceneNode);
    }

    emit onTemplatesPopulated();
}
QMultiMap<DesignProjectTemplates::DesignProjectViewType, DiagramSceneNode *> * DesignProjectTemplates::getAllDesignProjectNodesByProjectViewType()
{
    return m_AllDesignProjectNodesByProjectViewType;
}
QString DesignProjectTemplates::getDesignProjectViewTypeAsString(DesignProjectTemplates::DesignProjectViewType viewType)
{
    //TODOopt: DiagramSceneNode should accept the label in it's constructor and we should be able to access it from here without a switch statement
    switch(viewType)
    {
    case DesignProjectTemplates::ClassDiagramViewType:
        return QString("Class Diagram");
        break;
    case DesignProjectTemplates::UseCaseViewType:
        return QString("Use Case");
        break;
    }
    return QString();
}
