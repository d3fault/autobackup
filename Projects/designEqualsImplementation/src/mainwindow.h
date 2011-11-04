#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QHBoxLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAction>
#include <QTabWidget>

#include <openprojectsmanager.h>
#include <designproject.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    //gui
    QWidget *m_LeftPane;
    QTabWidget *m_ProjectTabWidget;
    //members
    OpenProjectsManager *m_OpenProjectsManager;
    //methods
    void createActions();
    void createLeftPane();
    void createProjectTabWidget();

    //actions
    QAction *m_NewProjectAction;
    QAction *m_NewClassAction;
    QAction *m_NewUseCaseAction;
private slots:
    void handleProjectOpened(DesignProject*);
    void handleProjectTabChanged(int);
    //actions triggered
    void handleNewProjectAction();
    void handleNewClassAction();
    void handleNewUseCaseAction();
};

#endif // MAINWINDOW_H
