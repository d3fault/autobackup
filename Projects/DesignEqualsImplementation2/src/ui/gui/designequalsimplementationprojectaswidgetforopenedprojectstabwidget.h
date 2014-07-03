#ifndef DESIGNEQUALSIMPLEMENTATIONPROJECTASWIDGETFOROPENEDPROJECTSTABWIDGET_H
#define DESIGNEQUALSIMPLEMENTATIONPROJECTASWIDGETFOROPENEDPROJECTSTABWIDGET_H

#include <QWidget>

class QTabWidget;

class DesignEqualsImplementationProject;
class DesignEqualsImplementationUseCase;

class DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget(DesignEqualsImplementationProject *designEqualsImplementationProject, QWidget *parent = 0);
private:
    QTabWidget *m_ClassDiagramAndUseCasesTabWidget;
    DesignEqualsImplementationProject *m_DesignEqualsImplementationProject;
private slots:
    void handleUseCaseAdded(DesignEqualsImplementationUseCase *useCase);
};

#endif // DESIGNEQUALSIMPLEMENTATIONPROJECTASWIDGETFOROPENEDPROJECTSTABWIDGET_H
