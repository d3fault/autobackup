#ifndef DIAGRAMSCENENODE_H
#define DIAGRAMSCENENODE_H

#include <QGraphicsPolygonItem>

#include "../ClassesThatRepresentProjectBeingCreated/designprojecttemplates.h"

class DiagramSceneNode : public QGraphicsPolygonItem
{
public:
    //thinking about abstracting all this type logic into it's own class that HAS this class as a "graphicalRepresentation();" member/method. edit: decided to just base it off this class directly
    //TODOreq: are these enums still necessary now that each one has it's own object type based off this?
    //enum DiagramSceneType { InvalidDiagramSceneType, ClassDiagramType, UseCaseType };
    //enum ClassDiagramTypes { InvalidClassDiagramType, FrontEnd, Backend };
    //enum UseCaseDiagramTypes { InvalidUseCaseType, Actor, FrontEnd, Backend };
    DiagramSceneNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType);
    int getUniqueId();
    DesignProjectTemplates::DesignProjectViewType getViewType();
    virtual QString getNodeTypeAsString()=0;
private:
    int m_UniqueId;
    DesignProjectTemplates::DesignProjectViewType m_ViewType;
};

#endif // DIAGRAMSCENENODE_H
