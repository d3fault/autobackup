#ifndef DESIGNEQUALSIMPLEMENTATIONPROJECTASWIDGETFOROPENEDPROJECTSTABWIDGET_H
#define DESIGNEQUALSIMPLEMENTATIONPROJECTASWIDGETFOROPENEDPROJECTSTABWIDGET_H

#include <QWidget>

class DesignEqualsImplementationProject;

class DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget(DesignEqualsImplementationProject *designEqualsImplementationProject, QWidget *parent = 0);
private:
    DesignEqualsImplementationProject *m_DesignEqualsImplementationProject;
};

#endif // DESIGNEQUALSIMPLEMENTATIONPROJECTASWIDGETFOROPENEDPROJECTSTABWIDGET_H
