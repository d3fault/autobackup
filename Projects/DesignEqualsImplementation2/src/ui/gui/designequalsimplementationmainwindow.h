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
    QAction *m_SaveProjectAction;
    QAction *m_OpenProjectAction;
    QAction *m_RenameProjectAction;
    QAction *m_NewUseCaseAction;
    QAction *m_GenerateSourceCodeAction;
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

    void decorateActions();

    void setClassDiagramToolsDisabled(bool disabled);
    void setUseCaseToolsDisabled(bool disabled);

    void addUseCaseToAllUseCasesListWidget(DesignEqualsImplementationUseCase *newUseCase);
signals:
    void newProjectRequested();
    void saveProjectRequested(DesignEqualsImplementationProject *projectToSave, const QString &projectFilePath);
    void openExistingProjectRequested(const QString &projectFilePath);
    void newUseCaseRequested();
    void mouseModeChanged(DesignEqualsImplementationMouseModeEnum newMouseMode);
public slots:
    void handleE(const QString &msg);
    void handleProjectOpened(DesignEqualsImplementationProject *project);
private slots:
    void handleSaveRequested();
    void handleOpenProjectActionTriggered();
#if !(QT_VERSION < QT_VERSION_CHECK(5, 2, 0))
    void handleProjectTabDoubleClicked(int indexOfTab);
#endif
    void handleRenameProjectActionTriggered();
    void handleNewUseCaseActionTriggered();
    void handleProjectTabWidgetOrClassDiagramAndUseCasesTabWidgetCurrentTabChanged();
    void handleUseCaseAdded(DesignEqualsImplementationUseCase *newUseCase);
    void handleAllUseCasesListWidgetItemDoubleClicked(QListWidgetItem *doubleClickedListWidgetItem);
    void doMouseModeChange();
    void handleGenerateSourceCodeActionTriggered();
};

#endif // DESIGNEQUALSIMPLEMENTATIONMAINWINDOW_H
