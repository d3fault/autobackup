#include "templateviewtab.h"

TemplateViewTab::TemplateViewTab(DesignProjectTemplates::DesignProjectViewType viewType) :
    m_ViewType(viewType)
{
    m_TabLabel = DesignProjectTemplates::getDesignProjectViewTypeAsString(viewType);
    m_ButtonGroup = new QButtonGroup();
    m_ButtonGroup->setExclusive(false);
}

//TODOreq: this stuff might not be needed anymore... changed design since. idk.
#if 0
void TemplateViewTab::handleNodeAdded(DesignProjectTemplates::DesignProjectViewType addedNodeViewType, DiagramSceneNode* addedNode)
{
    if(m_ViewType != addedNodeViewType)
        return; //all tabs get the signal, this is their filter //TODOmegaopt: figure out if you can make it so only the right tabs get the signal
    //megaopt means it has less priority than an optimization. it doesn't seem at all necessary at this time.


    //add the node to our button group + layout or whatever
    //might have to change the layout and then re-set it to get the add to show up? idfk
}
#endif
QString TemplateViewTab::getTabLabel()
{
    return m_TabLabel;
}

QButtonGroup * TemplateViewTab::getButtonGroup()
{
    return m_ButtonGroup;
}
