#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QHBoxLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAction>
#include <QTabWidget>
#include <QMenu>
#include <QMenuBar>
#include <QButtonGroup>

#include <projecttab.h>
#include <designproject.h>
#include <diagramscenenode.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    //gui
    QWidget *m_LeftPane;
    QTabWidget *m_ProjectTabWidgetContainer;
    QButtonGroup *m_ClassDiagramNodeButtonGroup;
    QButtonGroup *m_UseCaseNodeButtonGroup;
    //members
    DesignProject *m_CurrentProject;
    bool m_Failed;
    //methods
    void createActions();
    void createMenus();
    void createLeftPane();
    void createNodeButtonGroups();
    void createProjectTabWidget();

    //actions
    QAction *m_NewProjectAction;
    QAction *m_NewClassAction;
    QAction *m_NewUseCaseAction;
private slots:
    void handleProjectTabChanged(int);
    void handleButtonGroupButtonClicked(int);
    //actions triggered
    void handleNewProjectAction();
    void handleNewUseCaseAction();
signals:
    void e(const QString &);
};

#endif // MAINWINDOW_H
