#ifndef DESIGNEQUALSIMPLEMENTATIONMAINWINDOW_H
#define DESIGNEQUALSIMPLEMENTATIONMAINWINDOW_H

#include <QMainWindow>
#include <QList>

class QTabWidget;
class QAction;

class DesignEqualsImplementationProject;

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

    void createActions();
    void createMenu();
    void createToolbars();
    void createDockWidgets();
signals:
    void newProjectRequested();
    void openExistingProjectRequested(const QString &projectFilePath);
public slots:
    void handleProjectOpened(DesignEqualsImplementationProject *project);
private slots:
    void handleOpenProjectActionTriggered();
};

#endif // DESIGNEQUALSIMPLEMENTATIONMAINWINDOW_H
