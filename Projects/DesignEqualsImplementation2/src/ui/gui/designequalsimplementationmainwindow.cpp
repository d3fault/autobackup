#include "designequalsimplementationmainwindow.h"

#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QDockWidget>
#include <QTabWidget>
#include <QAction>
#include <QMutexLocker>

#include "../../designequalsimplementation.h"
#include "../../designequalsimplementationproject.h"
#include "umlitemswidget.h"
#include "designequalsimplementationprojectaswidgetforopenedprojectstabwidget.h"

#define DesignEqualsImplementationMainWindow_USER_VISIBLE_NAME "Design = Implementation" //thought about changing this to "Implementation = Design;" (the ordering change is significant, and the semi-colon is an nod)

//fucking QWidget::addActions didn't do what I expected... and I don't even get how QToolBar/Menu are using a NON-VIRTUAL method with identical name "addAction"
//#define DesignEqualsImplementationMainWindow_ADD_MY_ACTIONS(menuOrToolBarWithAddActionMethod)


//TODOreq: a toolbar that changes based on which kind of project view tab is open. ALWAYS have the "move" (mouse icon) button on far left regardless of toolbar's other contents. in class diagram view, some other buttons are inheritence arrows. in use case view, some other buttons are "signal/slot connection activation" arrows. when in any "arrow" mode, pressing the mouse "move" thing is your way of cancelling out (like tons of apps do)
//TODOreq: zooming in and out should only shrink/enlarge the objects ("class" uml object, for example). arrow width (or really any object's width tbh) and font size should always stay the same (but text can re-wrap/whatever based on new object size)
//TODOoptional: the class diagram view could have an opt-in "show connections between classes". It is opt-in because really such connections are use-case dependent and so the view may confuse more than help. A "connection" is made behind the scenes implicitly whenever an arrow is drawn from one class to the other in a use case view (that arrow/line is a "signal/slot connection activation")
//TODOreq: class diagram tab can never be closed, but makes sense that use case tabs can be opened/closed at will. Still need to decide where the full list of use cases would go though
//TODOreq: in use case view, the lifeline/object that your mouse is nearest to shows a red dot for where the next "statement" will go should you draw a line on it. It is useful to know if a signal emission/slot invocation will go before/after a different one. If there is only one slot invocation line connecting ('from the left') to your lifeline/object, that red dot is ALWAYS 'just below' the slot invocation that brought you to your current context (since it can't go below it). Such "red dots" are VECTOR and snappy, it should be instantly clear whether or not an arrow drawn will come before/after other ones. TODOreq: lines (statements ("signal/slot connection activations")) are moveable ofc
//TODOreq: "left" and "right" direction arrows that remember previous projects/use-cases+class-diagrams viewed, JUST LIKE Qt Creator :-P
DesignEqualsImplementationMainWindow::DesignEqualsImplementationMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(DesignEqualsImplementationMainWindow_USER_VISIBLE_NAME);
    createMenu(); //File
    createToolbar(); //Move/"Arrow" mode
    createDockWidgets(); //TODOreq: UML drag-drop objects, list of drag-drop objects when in use case view, list of use cases for opening their tab
    setCentralWidget(m_OpenProjectsTabWidget = new QTabWidget());
}
DesignEqualsImplementationMainWindow::~DesignEqualsImplementationMainWindow()
{ }
void DesignEqualsImplementationMainWindow::createActions()
{
    //Project Operations
    m_NewProjectAction = new QAction(tr("&New Project"), this);
    m_OpenProjectAction = new QAction(tr("&Open Open"), this);
    m_NewUseCaseAction = new QAction(tr("&New Use Case"), this);
    connect(m_NewProjectAction, SIGNAL(triggered()), this, SIGNAL(newProjectRequested()));
    connect(m_OpenProjectAction, SIGNAL(triggered()), this, SLOT(handleOpenProjectActionTriggered()));
    connect(m_NewUseCaseAction, SIGNAL(triggered()), this, SLOT(handleNewUseCaseActionTriggered()));

    //Main Toolbar Actions
    m_MoveMousePointerDefaultAction = new QAction(tr("&Move Mode"), this);
    m_DrawSignalSlotConnectionActivationArrowsAction = new QAction(tr("&Signals/Slots Connection Activation Arrows Mode"), this);
    connect(m_MoveMousePointerDefaultAction, SIGNAL(triggered()), this, SLOT(handleMoveMousePointerDefaultActionTriggered()));
    connect(m_DrawSignalSlotConnectionActivationArrowsAction, SIGNAL(triggered()), this, SLOT(handleDrawSignalSlotConnectionActivationArrowsActionTriggered()));
}
void DesignEqualsImplementationMainWindow::createMenu()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QMenu *fileNew = fileMenu->addMenu(tr("&New"));
    fileNew->addAction(m_NewProjectAction);
    fileNew->addAction(m_NewUseCaseAction);
    fileMenu->addAction(m_OpenProjectAction);
}
void DesignEqualsImplementationMainWindow::createToolbar()
{
    QToolBar *fileNewMenuEquivalentToolbar = addToolBar(DesignEqualsImplementationMainWindow_USER_VISIBLE_NAME " Project Operations Toolbar");
    fileNewMenuEquivalentToolbar->addAction(m_NewProjectAction);
    fileNewMenuEquivalentToolbar->addAction(m_OpenProjectAction);
    fileNewMenuEquivalentToolbar->addAction(m_NewUseCaseAction);

    QToolBar *mainToolbar = addToolBar(DesignEqualsImplementationMainWindow_USER_VISIBLE_NAME " Main Toolbar");
    mainToolbar->addAction(m_MoveMousePointerDefaultAction);
    mainToolbar->addAction(m_DrawSignalSlotConnectionActivationArrowsAction);
}
void DesignEqualsImplementationMainWindow::createDockWidgets()
{
    QDockWidget *umlDockWidget = new QDockWidget(tr("UML Items"), this);
    umlDockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    umlDockWidget->setWidget(new UmlItemsWidget(umlDockWidget));
    addDockWidget(Qt::LeftDockWidgetArea, umlDockWidget, Qt::Vertical);
}
//TODOreq: [backend] project is not thread safe to access directly, so check the source to make sure it's used properly (or i could mutex protect the DesignEqualsImplementationProject itself to KISS, undecided as of now)
void DesignEqualsImplementationMainWindow::handleProjectOpened(DesignEqualsImplementationProject *project)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    m_OpenProjectsTabWidget->addTab(new DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget(project, m_OpenProjectsTabWidget), project->Name);
}
void DesignEqualsImplementationMainWindow::handleOpenProjectActionTriggered()
{
    //TODOreq: file dialog and then
    emit openExistingProjectRequested(QString());
}
