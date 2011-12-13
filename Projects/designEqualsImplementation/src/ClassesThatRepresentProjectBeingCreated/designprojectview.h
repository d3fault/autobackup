#ifndef DESIGNPROJECTVIEW_H
#define DESIGNPROJECTVIEW_H

#include <QObject>
#include <QPointF>

#include "designprojecttemplates.h"
#include "Gui/diagramscenenode.h"
#include "../projectcontroller.h"

class DesignProjectView : public QObject
{
    Q_OBJECT
public:
    explicit DesignProjectView(QString useCaseName);
    explicit DesignProjectView();
    QString getProjectViewName();

    void addPendingNodeAt(QPointF pointNodeIsAtInScene);
private:
    void privConstructor(QString projectViewName, DesignProjectTemplates::DesignProjectViewType projectViewType);
    QString m_ProjectViewName;
    DesignProjectTemplates::DesignProjectViewType m_ProjectViewType;
    bool m_ThisViewIsClassDiagram;

    //TODOreq: parcelize
    QMap<DiagramSceneNode*,QPointF> *m_ListOfNodes;
signals:
    void nodeAdded(DiagramSceneNode *node);

public slots:

};

#endif // DESIGNPROJECTVIEW_H
