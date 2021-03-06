#ifndef CLASSDIAGRAMFRONTENDNODE_H
#define CLASSDIAGRAMFRONTENDNODE_H

#include "../Gui/diagramscenenode.h"

class ClassDiagramFrontEndNode : public DiagramSceneNode
{
public:
    ClassDiagramFrontEndNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType);
    virtual QString getNodeTypeAsString();
    virtual DesignProjectTemplates::DesignProjectViewNodeType getNodeType();
    virtual void drawMyPolygon();
};

#endif // CLASSDIAGRAMFRONTENDNODE_H
