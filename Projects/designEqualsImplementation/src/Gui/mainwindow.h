#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAction>
#include <QTabWidget>
#include <QMenu>
#include <QMenuBar>
#include <QButtonGroup>
#include <QToolButton>
#include <QLabel>
#include <QRect>

#include <QList>
#include <QMultiMap>
#include <QListIterator>

#include "WidgetsThatRepresentIndividualTabItems/projecttab.h"
#include "ClassesThatRepresentProjectBeingCreated/designproject.h"
#include "diagramscenenode.h"
#include "ClassesThatRepresentProjectBeingCreated/designprojecttemplates.h"
#include "WidgetsThatRepresentIndividualTabItems/templateviewtab.h"
#include "dragdropdiagramscene.h"
#include "../StateMachine/modesingleton.h"

#include <QStyle>
//^TODOreq: use icon.image() instead of these placeholder stock icons

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    //gui
    QWidget *m_LeftPane;
    QTabWidget *m_ProjectTabWidgetContainer; //the tab widget containing the project tabs
    QTabWidget *m_UseCaseAndClassDiagramViewsNodesTemplateSelectorButtonGroupTabWidget; //the tab widget containing the node elements over on the left
    //members
    DesignProject *m_CurrentProject;
    bool m_Failed;
    //methods
    void createActions();
    void createMenus();
    void createLeftPane();
    void createNodeButtonGroups();
    void createProjectTabWidget();
    QWidget *createTemplateNodeButtonWidget(DiagramSceneNode *diagramSceneNode, QToolButton *buttonToConfigureAndUseInLayout);

    void setAllToolboxButtonsToNotCheckedExcept(int buttonIdofButtonJustClicked);

    //actions
    QAction *m_NewProjectAction;
    QAction *m_NewClassAction; //does this come out or does graphicsscene trigger this in it's itemAdded signal handler?
    QAction *m_NewUseCaseAction;
private slots:
    void handleProjectTabChanged(int);
    void handleButtonGroupButtonClicked(int);
    void handleTemplatesPopulated();
    void handleModeChanged(ModeSingleton::Mode newMode);
    //actions triggered
    void handleNewProjectAction();
    void handleNewUseCaseAction();
signals:
    void e(const QString &);
};

#endif // MAINWINDOW_H
