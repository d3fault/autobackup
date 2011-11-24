#include "templateviewtab.h"

TemplateViewTab::TemplateViewTab(DesignProjectTemplates::DesignProjectViewType viewType) :
    m_ViewType(viewType)
{
}
void TemplateViewTab::handleNodeAdded(DesignProjectTemplates::DesignProjectViewType addedNodeViewType, DiagramSceneNode* addedNode)
{
    if(m_ViewType != addedNodeViewType)
        return; //all tabs get the signal, this is their filter //TODOmegaopt: figure out if you can make it so only the right tabs get the signal


    //add the node to our button group + layout or whatever
    //might have to change the layout and then re-set it to get the add to show up? idfk
}
