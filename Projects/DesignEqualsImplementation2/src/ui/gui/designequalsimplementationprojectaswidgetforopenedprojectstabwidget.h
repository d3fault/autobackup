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
    explicit DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget(DesignEqualsImplementationProject *project, QWidget *parent = 0);
    QTabWidget *classDiagramAndUseCasesTabWidget();
    DesignEqualsImplementationProject *designEqualsImplementationProject();
private:
    QTabWidget *m_ClassDiagramAndUseCasesTabWidget;
    DesignEqualsImplementationProject *m_DesignEqualsImplementationProject;
signals:
    void newUseCaseRequested();
private slots:
    void requestNewUseCase();
    void handleUseCaseAdded(DesignEqualsImplementationUseCase *useCase);
};

#endif // DESIGNEQUALSIMPLEMENTATIONPROJECTASWIDGETFOROPENEDPROJECTSTABWIDGET_H
