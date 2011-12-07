#ifndef USECASEBACKENDNODE_H
#define USECASEBACKENDNODE_H

#include "../Gui/diagramscenenode.h"

class UseCaseBackEndNode : public DiagramSceneNode
{
public:
    UseCaseBackEndNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType);
    virtual QString getNodeTypeAsString();
};

#endif // USECASEBACKENDNODE_H