#ifndef USECASEFRONTENDNODE_H
#define USECASEFRONTENDNODE_H

#include "../Gui/diagramscenenode.h"

class UseCaseFrontEndNode : public DiagramSceneNode
{
public:
    UseCaseFrontEndNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType);
    virtual QString getNodeTypeAsString();
    virtual DesignProjectTemplates::DesignProjectViewNodeType getNodeType();
    virtual void drawMyPolygon();
};

#endif // USECASEFRONTENDNODE_H
