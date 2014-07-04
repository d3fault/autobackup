#ifndef DESIGNEQUALSIMPLEMENTATIONMAINWINDOW_H
#define DESIGNEQUALSIMPLEMENTATIONMAINWINDOW_H

#include <QMainWindow>
#include <QList>

class QTabWidget;
class QAction;
class QStackedWidget;

class DesignEqualsImplementationProject;
class ClassDiagramUmlItemsWidget;
class UseCaseUmlItemsWidget;

class DesignEqualsImplementationMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    DesignEqualsImplementationMainWindow(QWidget *parent = 0);
    ~DesignEqualsImplementationMainWindow();
private:
    QTabWidget *m_OpenProjectsTabWidget;

    //Actions
    QAction *m_NewProjectAction;
    QAction *m_OpenProjectAction;
    QAction *m_NewUseCaseAction;
    //More actions
    QAction *m_MoveMousePointerDefaultAction;
    QAction *m_DrawSignalSlotConnectionActivationArrowsAction;

    //Left dock widget
    QStackedWidget *m_UmlStackedDockWidget;
    ClassDiagramUmlItemsWidget *m_ClassDiagramUmlItemsWidget;
    UseCaseUmlItemsWidget *m_UseCaseUmlItemsWidget;

    void createActions();
    void createMenu();
    void createToolbars();
    void createDockWidgets();

    void setClassDiagramToolsDisabled(bool disabled);
    void setUseCaseToolsDisabled(bool disabled);
signals:
    void newProjectRequested();
    void openExistingProjectRequested(const QString &projectFilePath);
    void newUseCaseRequested();
public slots:
    void handleProjectOpened(DesignEqualsImplementationProject *project);
private slots:
    void handleOpenProjectActionTriggered();
    void handleNewUseCaseActionTriggered();
    void handleProjectTabWidgetOrClassDiagramAndUseCasesTabWidgetCurrentTabChanged();
};

#endif // DESIGNEQUALSIMPLEMENTATIONMAINWINDOW_H
