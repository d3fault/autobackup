#ifndef USECASEACTORNODE_H
#define USECASEACTORNODE_H

#include "../Gui/diagramscenenode.h"

class UseCaseActorNode : public DiagramSceneNode
{
public:
    UseCaseActorNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType);
    virtual QString getNodeTypeAsString();
    virtual DesignProjectTemplates::DesignProjectViewNodeType getNodeType();
    virtual void drawMyPolygon();
};

#endif // USECASEACTORNODE_H
