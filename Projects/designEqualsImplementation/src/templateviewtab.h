#ifndef TEMPLATEVIEWTAB_H
#define TEMPLATEVIEWTAB_H

#include <QWidget>
#include <designprojecttemplates.h>
#include <diagramscenenode.h>

class TemplateViewTab : public QWidget
{
    Q_OBJECT
public:
    explicit TemplateViewTab(DesignProjectTemplates::DesignProjectViewType viewType);
private:
    DesignProjectTemplates::DesignProjectViewType m_ViewType;
signals:

public slots:
    void handleNodeAdded(DesignProjectTemplates::DesignProjectViewType, DiagramSceneNode*);
};

#endif // TEMPLATEVIEWTAB_H
