#ifndef CLASSDIAGRAMBACKENDNODE_H
#define CLASSDIAGRAMBACKENDNODE_H

#include "../Gui/diagramscenenode.h"

class ClassDiagramBackEndNode : public DiagramSceneNode
{
public:
    ClassDiagramBackEndNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType);
};

#endif // CLASSDIAGRAMBACKENDNODE_H
