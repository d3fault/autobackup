#ifndef PROJECTCONTROLLER_H
#define PROJECTCONTROLLER_H

#include <QObject>

class DesignProject;
class DiagramSceneNode;

class ProjectController : public QObject
{
    Q_OBJECT
public:
    static ProjectController *Instance();

    DesignProject *getCurrentProject();
    void setCurrentProject(DesignProject *newCurrentProject);
    void setPendingNode(DiagramSceneNode *node);
    DiagramSceneNode *getPendingNode();
private:
    ProjectController();
    static ProjectController *m_pInstance;

    DesignProject *m_CurrentProject;
    DiagramSceneNode *m_PendingNode;
signals:
    void currentProjectChanged(DesignProject *newCurrentProject);

public slots:

};

#endif // PROJECTCONTROLLER_H
