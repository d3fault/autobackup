#include "designequalsimplementationmainwindow.h"

#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QDockWidget>
#include <QStackedWidget>
#include <QTabWidget>
#include <QAction>
#include <QMutexLocker>

#include "../../designequalsimplementation.h"
#include "../../designequalsimplementationproject.h"
#include "classdiagramumlitemswidget.h"
#include "usecaseumlitemswidget.h"
#include "designequalsimplementationprojectaswidgetforopenedprojectstabwidget.h"
#include "designequalsimplementationguicommon.h"

#define DesignEqualsImplementationMainWindow_USER_VISIBLE_NAME "Design = Implementation" //thought about changing this to "Implementation = Design;" (the ordering change is significant, and the semi-colon is an nod)

//fucking QWidget::addActions didn't do what I expected... and I don't even get how QToolBar/Menu are using a NON-VIRTUAL method with identical name "addAction"
//#define DesignEqualsImplementationMainWindow_ADD_MY_ACTIONS(menuOrToolBarWithAddActionMethod)


//TODOreq: a toolbar that changes based on which kind of project view tab is open. ALWAYS have the "move" (mouse icon) button on far left regardless of toolbar's other contents. in class diagram view, some other buttons are inheritence arrows. in use case view, some other buttons are "signal/slot connection activation" arrows. when in any "arrow" mode, pressing the mouse "move" thing is your way of cancelling out (like tons of apps do)
//TODOreq: zooming in and out should only shrink/enlarge the objects ("class" uml object, for example). arrow width (or really any object's width tbh) and font size should always stay the same (but text can re-wrap/whatever based on new object size)
//TODOoptional: the class diagram view could have an opt-in "show connections between classes". It is opt-in because really such connections are use-case dependent and so the view may confuse more than help. A "connection" is made behind the scenes implicitly whenever an arrow is drawn from one class to the other in a use case view (that arrow/line is a "signal/slot connection activation")
//TODOreq: class diagram tab can never be closed, but makes sense that use case tabs can be opened/closed at will. Still need to decide where the full list of use cases would go though
//TODOreq: in use case view, the lifeline/object that your mouse is nearest to shows a red dot for where the next "statement" will go should you draw a line on it. It is useful to know if a signal emission/slot invocation will go before/after a different one. If there is only one slot invocation line connecting ('from the left') to your lifeline/object, that red dot is ALWAYS 'just below' the slot invocation that brought you to your current context (since it can't go below it). Such "red dots" are VECTOR and snappy, it should be instantly clear whether or not an arrow drawn will come before/after other ones. TODOreq: lines (statements ("signal/slot connection activations")) are moveable ofc
//TODOreq: "left" and "right" direction arrows that remember previous projects/use-cases+class-diagrams viewed, JUST LIKE Qt Creator :-P
//TODOoptional: a first run wizard teaching them how to design + execute hello world (or something more interesting). ex: "click and drag one of these classes here" (class creation), <insert class population instructions>, "ok now add a use case", "ok now draw line from here to here". should ideally be all visual and shit pointing to what the user needs to do next
DesignEqualsImplementationMainWindow::DesignEqualsImplementationMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(DesignEqualsImplementationMainWindow_USER_VISIBLE_NAME);
    createActions();
    createMenu(); //File
    createToolbars(); //Move/"Arrow" mode
    createDockWidgets(); //TODOreq: UML drag-drop objects, list of drag-drop objects when in use case view, list of use cases for opening their tab
    setCentralWidget(m_OpenProjectsTabWidget = new QTabWidget());

    connect(m_OpenProjectsTabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleProjectTabWidgetOrClassDiagramAndUseCasesTabWidgetCurrentTabChanged()));
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
void DesignEqualsImplementationMainWindow::createToolbars()
{
    QToolBar *fileNewMenuEquivalentToolbar = addToolBar(DesignEqualsImplementationMainWindow_USER_VISIBLE_NAME " Project Operations Toolbar");
    fileNewMenuEquivalentToolbar->addAction(m_NewProjectAction);
    fileNewMenuEquivalentToolbar->addAction(m_OpenProjectAction);
    fileNewMenuEquivalentToolbar->addAction(m_NewUseCaseAction);

    addToolBarBreak();

    QToolBar *mainToolbar = addToolBar(DesignEqualsImplementationMainWindow_USER_VISIBLE_NAME " Main Toolbar");
    mainToolbar->addAction(m_MoveMousePointerDefaultAction);
    mainToolbar->addAction(m_DrawSignalSlotConnectionActivationArrowsAction);
}
void DesignEqualsImplementationMainWindow::createDockWidgets()
{
    QDockWidget *dockWidget = new QDockWidget(tr("Available Items"), this);
    dockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    m_UmlStackedDockWidget = new QStackedWidget();
    m_ClassDiagramUmlItemsWidget = new ClassDiagramUmlItemsWidget();
    m_UseCaseUmlItemsWidget = new UseCaseUmlItemsWidget();
    m_UmlStackedDockWidget->addWidget(m_ClassDiagramUmlItemsWidget);
    m_UmlStackedDockWidget->addWidget(m_UseCaseUmlItemsWidget);

    dockWidget->setWidget(m_UmlStackedDockWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget, Qt::Vertical);

    //defaults
    setClassDiagramToolsDisabled(false);
    setClassDiagramToolsDisabled(true);
}
void DesignEqualsImplementationMainWindow::setClassDiagramToolsDisabled(bool disabled)
{
    //an inheritence arrow drawer (that same association should also be specifiable in the "class editor" widget... makes no difference)
}
void DesignEqualsImplementationMainWindow::setUseCaseToolsDisabled(bool disabled)
{
    //TODOoptional: a run-time option/setting could be whether or not to setDisabled or setHidden for all these toolbars/actions... but the default should be setDisabled
    m_DrawSignalSlotConnectionActivationArrowsAction->setDisabled(disabled);
}
//TODOreq: [backend] project is not thread safe to access directly, so check the source to make sure it's used properly (or i could mutex protect the DesignEqualsImplementationProject itself to KISS, undecided as of now)
void DesignEqualsImplementationMainWindow::handleProjectOpened(DesignEqualsImplementationProject *project)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget *designEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget = new DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget(project);
    int tabIndex = m_OpenProjectsTabWidget->addTab(designEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget, project->Name);
    connect(designEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget->classDiagramAndUseCasesTabWidget(), SIGNAL(currentChanged(int)), this, SLOT(handleProjectTabWidgetOrClassDiagramAndUseCasesTabWidgetCurrentTabChanged()));
    m_OpenProjectsTabWidget->setCurrentIndex(tabIndex);
}
void DesignEqualsImplementationMainWindow::handleOpenProjectActionTriggered()
{
    //TODOreq: file dialog and then
    emit openExistingProjectRequested(QString());
}
void DesignEqualsImplementationMainWindow::handleNewUseCaseActionTriggered()
{
    DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget *currentProjectTab = static_cast<DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget*>(m_OpenProjectsTabWidget->currentWidget());
    QMetaObject::invokeMethod(currentProjectTab, "requestNewUseCase");
}
void DesignEqualsImplementationMainWindow::handleProjectTabWidgetOrClassDiagramAndUseCasesTabWidgetCurrentTabChanged()
{
    //NOTE: the reason we don't use the "new tab index" arg from the emitting signalS is because both the "project tabs" changing and the "uml and use cases tabs" changing trigger this slot. not a huge deal since all we're doing is determining which set of tools to present (class diagram vs. use case).

    //TODOreq: don't 'double show'. as in, don't change tools if the correct ones are already being shown

    //If I ever allow tab re-ordering and class diagram doesn't doesn't have tab index == 0, i can still easily check what kind of tab is the new current by doing a qobject_cast ;-P
    if(static_cast<DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget*>(m_OpenProjectsTabWidget->currentWidget())->classDiagramAndUseCasesTabWidget()->currentIndex() == 0)
    {
        //show class diagram tools
        setClassDiagramToolsDisabled(false);
        setUseCaseToolsDisabled(false);
        m_UmlStackedDockWidget->setCurrentWidget(m_ClassDiagramUmlItemsWidget);
    }
    else
    {
        //show use case tools
        //To delete, to hide, or (???), that is the question. decided to setDisabled(true), since fuck yea Qt is smart enough to disable all the gui variants when disabling an action <3 <3. TODOoptional: hide them visually, but idk i kinda like that they stay in the GUI and just get grayed out. Keeps the end user more "in the loop" and they are more likely to recognize that tools are being enabled/disabled as they change tabs (when a button is replaced and the new one takes the old position exactly, it's difficult for even coders to notice the change)
        //TODOreq: There's still the dockwidget stuff that needs to change guh, in that case i think i do want to hide it visually (since it takes so much space, and since the changing is easily noticeable)
        setClassDiagramToolsDisabled(true);
        setUseCaseToolsDisabled(false);
        m_UmlStackedDockWidget->setCurrentWidget(m_UseCaseUmlItemsWidget);
    }

    //TODOreq: when the PROJECT tab changes, we also need to change the "available use cases" (when in class diagram view (double clicking opens it in tab)) and "available classes" (when in use case view (for dragging onto use case))
}
