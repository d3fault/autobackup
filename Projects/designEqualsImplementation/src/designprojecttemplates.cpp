#include "designprojecttemplates.h"

#include "DiagramSceneNodes/classdiagramfrontendnode.h"
#include "DiagramSceneNodes/classdiagrambackendnode.h"

#include "DiagramSceneNodes/usecaseactornode.h"
#include "DiagramSceneNodes/usecasefrontendnode.h"
#include "DiagramSceneNodes/usecasebackendnode.h"

DesignProjectTemplates::DesignProjectTemplates()
{
    //i'm a bit confused on how to do this design. i need to declare each of the underlying types as an object... but there is no reference to them EXCEPT when they are added to the list below
    //that's how the rest of the app finds out the project view types (class diagram, use case), and what diagramscenenodes that project view type supports (actor, frontend, backend, etc)

    //TODOopt: another class diagram scene node type can be USE CASE ENTRANCE. it puts a tiny actor next to where the use case first calls the front-end (use cases don't have to call a front-end.. but a user/actor DOES)
    //so maybe not a tiny actor... but a tiny "UC" or something

    AllDesignProjectNodesByProjectViewType = new QMultiMap<DesignProjectViewType, DesignProjectView*>();
    populateDesignProjectViewsAndTheirNodes();
}
void DesignProjectTemplates::populateDesignProjectViewsAndTheirNodes()
{
    //class diagram
    AllDesignProjectNodesByProjectViewType->insert(ClassDiagramType, new ClassDiagramFrontEndNode());
    AllDesignProjectNodesByProjectViewType->insert(ClassDiagramType, new ClassDiagramBackEndNode());

    //use case
    AllDesignProjectNodesByProjectViewType->insert(UseCaseType, new UseCaseActorNode());
    AllDesignProjectNodesByProjectViewType->insert(UseCaseType, new UseCaseFrontEndNode());
    AllDesignProjectNodesByProjectViewType->insert(UseCaseType, new UseCaseBackEndNode());
}
