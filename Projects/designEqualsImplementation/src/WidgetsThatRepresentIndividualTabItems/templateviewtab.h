#ifndef TEMPLATEVIEWTAB_H
#define TEMPLATEVIEWTAB_H

#include <QWidget>
#include <QButtonGroup>

#include "../ClassesThatRepresentProjectBeingCreated/designprojecttemplates.h"
#include "../Gui/diagramscenenode.h"

class TemplateViewTab : public QWidget
{
    Q_OBJECT
public:
    explicit TemplateViewTab(DesignProjectTemplates::DesignProjectViewType viewType);
    QButtonGroup *getButtonGroup();
    QString getTabLabel(); //TODOopt: make a base type for widgets in tabs that has this method, as i use it a lot. overridealble though...
private:
    DesignProjectTemplates::DesignProjectViewType m_ViewType;
    QButtonGroup *m_ButtonGroup;
    QString m_TabLabel;
};

#endif // TEMPLATEVIEWTAB_H
