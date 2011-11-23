#ifndef DIAGRAMSCENENODE_H
#define DIAGRAMSCENENODE_H

#include <QGraphicsPolygonItem>

class DiagramSceneNode : public QGraphicsPolygonItem
{
public:
    //thinking about abstracting all this type logic into it's own class that HAS this class as a "graphicalRepresentation();" member/method. edit: decided to just base it off this class directly
    //TODOreq: are these enums still necessary now that each one has it's own object type based off this?
    enum DiagramSceneType { InvalidDiagramSceneType, ClassDiagramType, UseCaseType };
    enum ClassDiagramTypes { InvalidClassDiagramType, FrontEnd, Backend };
    enum UseCaseDiagramTypes { InvalidUseCaseType, Actor, FrontEnd, Backend };
    DiagramSceneNode();
};

#endif // DIAGRAMSCENENODE_H
