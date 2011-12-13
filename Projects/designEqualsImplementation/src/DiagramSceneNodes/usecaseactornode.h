#ifndef USECASEACTORNODE_H
#define USECASEACTORNODE_H

#include "../Gui/diagramscenenode.h"

class UseCaseActorNode : public DiagramSceneNode
{
public:
    UseCaseActorNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType);
    virtual QString getNodeTypeAsString();
    virtual void drawMyPolygon();
    //virtual DiagramSceneNode *cloneSelf();
};

#endif // USECASEACTORNODE_H
