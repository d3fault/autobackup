#ifndef USECASEFRONTENDNODE_H
#define USECASEFRONTENDNODE_H

#include "../Gui/diagramscenenode.h"

class UseCaseFrontEndNode : public DiagramSceneNode
{
public:
    UseCaseFrontEndNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType);
    virtual QString getNodeTypeAsString();
};

#endif // USECASEFRONTENDNODE_H
