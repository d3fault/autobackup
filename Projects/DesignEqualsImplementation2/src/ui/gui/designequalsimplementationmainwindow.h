#ifndef DESIGNEQUALSIMPLEMENTATIONMAINWINDOW_H
#define DESIGNEQUALSIMPLEMENTATIONMAINWINDOW_H

#include <QMainWindow>
#include <QList>

#include "designequalsimplementationguicommon.h"

class QTabWidget;
class QAction;
class QStackedWidget;
class QListWidget;
class QListWidgetItem;

class DesignEqualsImplementationProject;
class DesignEqualsImplementationUseCase;
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

    //Top-Left uml stacked widget
    QStackedWidget *m_UmlStackedDockWidget;
    ClassDiagramUmlItemsWidget *m_ClassDiagramUmlItemsWidget;
    UseCaseUmlItemsWidget *m_UseCaseUmlItemsWidget;
    //Bottom-Left all use cases list widget
    QListWidget *m_AllUseCasesListWidget;

    int m_CurrentProjectTabIndex;

    void createActions();
    void createMenu();
    void createToolbars();
    void createDockWidgets();

    void setClassDiagramToolsDisabled(bool disabled);
    void setUseCaseToolsDisabled(bool disabled);

    void addUseCaseToAllUseCasesListWidget(DesignEqualsImplementationUseCase *newUseCase);
signals:
    void newProjectRequested();
    void openExistingProjectRequested(const QString &projectFilePath);
    void newUseCaseRequested();
    void mouseModeChanged(DesignEqualsImplementationMouseModeEnum newMouseMode);
public slots:
    void handleProjectOpened(DesignEqualsImplementationProject *project);
private slots:
    void handleOpenProjectActionTriggered();
    void handleNewUseCaseActionTriggered();
    void handleProjectTabWidgetOrClassDiagramAndUseCasesTabWidgetCurrentTabChanged();
    void handleUseCaseAdded(DesignEqualsImplementationUseCase *newUseCase);
    void handleAllUseCasesListWidgetItemDoubleClicked(QListWidgetItem *doubleClickedListWidgetItem);
    void doMouseModeChange();
};

#endif // DESIGNEQUALSIMPLEMENTATIONMAINWINDOW_H
