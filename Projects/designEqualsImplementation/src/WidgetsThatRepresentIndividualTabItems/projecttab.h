#ifndef PROJECTTAB_H
#define PROJECTTAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTabWidget>

#include "../ClassesThatRepresentProjectBeingCreated/designproject.h"
#include "projectviewtab.h"

class ProjectTab : public QWidget
{
    Q_OBJECT
public:
    explicit ProjectTab(DesignProject *project);
    DesignProject *getProject();
private:
    QVBoxLayout *m_Layout;
    QTabWidget *m_ProjectViewsTabContainer;
    DesignProject *m_Project;
    bool m_Failed;
signals:
    void e(const QString &);
private slots:
    void handleProjectViewsTabChanged(int);
    void handleProjectViewAdded(DesignProjectView*);
};

#endif // PROJECTTAB_H
