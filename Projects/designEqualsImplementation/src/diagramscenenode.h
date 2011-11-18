#ifndef DIAGRAMSCENENODE_H
#define DIAGRAMSCENENODE_H

#include <QGraphicsPolygonItem>

class DiagramSceneNode : public QGraphicsPolygonItem
{
public:
    enum DiagramSceneType { InvalidDiagramSceneType, ClassDiagramType, UseCaseType };
    enum ClassDiagramTypes { InvalidClassDiagramType, FrontEnd, Backend };
    enum UseCaseDiagramTypes { InvalidUseCaseType, Actor, FrontEnd, Backend };
    DiagramSceneNode();
};

#endif // DIAGRAMSCENENODE_H
