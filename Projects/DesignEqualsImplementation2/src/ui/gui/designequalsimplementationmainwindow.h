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
    QList<QAction*> m_ProjectOperationsActions;
    QList<QAction*> m_MainToolbarActions;

    void createActions();
    void createMenu();
    void createToolbar();
    void createDockWidgets();
signals:
    void newProjectRequested();
    void openExistingProjectRequested(const QString &projectFilePath);
public slots:
    void handleProjectOpened(DesignEqualsImplementationProject *project, const QString &projectName);
private slots:
    void handleOpenProjectActionTriggered();
};

#endif // DESIGNEQUALSIMPLEMENTATIONMAINWINDOW_H
