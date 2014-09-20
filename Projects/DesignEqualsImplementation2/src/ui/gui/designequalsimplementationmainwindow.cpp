#include "designequalsimplementationmainwindow.h"

#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QDockWidget>
#include <QStackedWidget>
#include <QTabWidget>
#include <QAction>
#include <QListWidget>
#include <QActionGroup> //and to think i wasted to much time with qbuttongroup/groupbox/etc in other projects!
#include <QCoreApplication>
#include <QFileDialog>
#include <QMessageBox>

#include <QDateTime> //temp/debug

#include "../../designequalsimplementationproject.h"
#include "classdiagramumlitemswidget.h"
#include "usecaseumlitemswidget.h"
#include "designequalsimplementationprojectaswidgetforopenedprojectstabwidget.h"
#include "renameprojectdialog.h"

#define DesignEqualsImplementationMainWindow_USER_VISIBLE_NAME tr("Design = Implementation") //thought about changing this to "Implementation = Design;" (the ordering change is significant, and the semi-colon is an nod) -- however i like the equality usage of the equal sign rather than assignment, in which case the word design coming first makes more sense (because the design does come before the implementation (most of the time, but especially in this app))

#define DesignEqualsImplementationMainWindow_DOCK_WIDGET_FEATURES (QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable)

//fucking QWidget::addActions didn't do what I expected... and I don't even get how QToolBar/Menu are using a NON-VIRTUAL method with identical name "addAction"
//#define DesignEqualsImplementationMainWindow_ADD_MY_ACTIONS(menuOrToolBarWithAddActionMethod)


//TODOreq: a toolbar that changes based on which kind of project view tab is open. ALWAYS have the "move" (mouse icon) button on far left regardless of toolbar's other contents. in class diagram view, some other buttons are inheritence arrows. in use case view, some other buttons are "signal/slot connection activation" arrows. when in any "arrow" mode, pressing the mouse "move" thing is your way of cancelling out (like tons of apps do)
//TODOreq: zooming in and out should only shrink/enlarge the objects ("class" uml object, for example). arrow width (or really any object's width tbh) and font size should always stay the same (but text can re-wrap/whatever based on new object size)
//TODOoptional: the class diagram view could have an opt-in "show connections between classes". It is opt-in because really such connections are use-case dependent and so the view may confuse more than help. A "connection" is made behind the scenes implicitly whenever an arrow is drawn from one class to the other in a use case view (that arrow/line is a "signal/slot connection activation")
//TODOreq: class diagram tab can never be closed, but makes sense that use case tabs can be opened/closed at will. Still need to decide where the full list of use cases would go though
//TODOreq: in use case view, the lifeline/object that your mouse is nearest to shows a red dot for where the next "statement" will go should you draw a line on it. It is useful to know if a signal emission/slot invocation will go before/after a different one. If there is only one slot invocation line connecting ('from the left') to your lifeline/object, that red dot is ALWAYS 'just below' the slot invocation that brought you to your current context (since it can't go below it). Such "red dots" are VECTOR and snappy, it should be instantly clear whether or not an arrow drawn will come before/after other ones. TODOreq: lines (statements ("signal/slot connection activations")) are moveable ofc
//TODOreq: "left" and "right" direction arrows that remember previous projects/use-cases+class-diagrams viewed, JUST LIKE Qt Creator :-P
//TODOoptional: a first run wizard teaching them how to design + execute hello world (or something more interesting). ex: "click and drag one of these classes here" (class creation), <insert class population instructions>, "ok now add a use case", "ok now draw line from here to here". should ideally be all visual and shit pointing to what the user needs to do next
//TODOreq: holding ctrl should put the mouse mode into arrow mode until it's released (if already in arrow mode, do nothing (and don't change out of it when ctrl released))
//TODOreq: when opening a project, if the current project is EMPTY, close the current empty project first (so only one is open)
//TODOoptional: when naming the project, there could be one/several fields below the "human readable" line edit that are auto-generated-but-editable. Like for example the top-most "project name" is typed in as "My Project", and then below it the name-of-resulting-binary line edit could auto generate "myproject" (like I said, editable still). It is basically the qmake TARGET
DesignEqualsImplementationMainWindow::DesignEqualsImplementationMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_CurrentProjectTabIndex(0)
{
    setWindowTitle(DesignEqualsImplementationMainWindow_USER_VISIBLE_NAME);
    createActions();
    createMenu(); //File
    createToolbars(); //Move/"Arrow" mode
    createDockWidgets(); //TODOreq: UML drag-drop objects, list of drag-drop objects when in use case view, list of use cases for opening their tab
    setCentralWidget(m_OpenProjectsTabWidget = new QTabWidget());

    connect(m_OpenProjectsTabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleProjectTabWidgetOrClassDiagramAndUseCasesTabWidgetCurrentTabChanged()));
    connect(m_AllUseCasesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(handleAllUseCasesListWidgetItemDoubleClicked(QListWidgetItem*)));
#if !(QT_VERSION < QT_VERSION_CHECK(5, 2, 0))
    connect(m_OpenProjectsTabWidget, SIGNAL(tabBarDoubleClicked(int)), this, SLOT(handleProjectTabDoubleClicked(int)));
#endif
}
DesignEqualsImplementationMainWindow::~DesignEqualsImplementationMainWindow()
{ }
void DesignEqualsImplementationMainWindow::createActions()
{
    //Project Operations
    m_NewProjectAction = new QAction(tr("New &Project"), this); //TODOreq: lots of these ampersand things have collisions lewl
    m_SaveProjectAction = new QAction(tr("&Save Project"), this); //TODOoptional: save as. TODOreq: periodic auto-saving to temp dir (which should not be in ram). perhaps a timeline of them with a (had:known-fix[..]) fixed-size cryptographic integrity chunk at the end for cheap "latest + not-load-corrupt-ever" power failure recovery. a better power failure recovery is keeping 3 computers in sync with each other and on independent power (each with ups). i'm surprised no such system is available and advertised. if i had more time on my hands i would make it and target openbsd/debian combo (perhaps using each as one of the 3 computers. i bet openbsd is the most stable. that last description was for a computer environment in general, not just designEquals
    m_OpenProjectAction = new QAction(tr("&Open Project"), this);
    m_RenameProjectAction = new QAction(tr("&Rename Project"), this);
    m_NewUseCaseAction = new QAction(tr("New &Use Case"), this);
    m_GenerateSourceCodeAction = new QAction(tr("&Generate Source Code"), this);
    connect(m_NewProjectAction, SIGNAL(triggered()), this, SIGNAL(newProjectRequested()));
    connect(m_SaveProjectAction, SIGNAL(triggered()), this, SLOT(handleSaveRequested()));
    connect(m_OpenProjectAction, SIGNAL(triggered()), this, SLOT(handleOpenProjectActionTriggered()));
    connect(m_RenameProjectAction, SIGNAL(triggered()), this, SLOT(handleRenameProjectActionTriggered()));
    connect(m_NewUseCaseAction, SIGNAL(triggered()), this, SLOT(handleNewUseCaseActionTriggered()));
    connect(m_GenerateSourceCodeAction, SIGNAL(triggered()), this, SLOT(handleGenerateSourceCodeActionTriggered()));

    //Main Toolbar Actions
    QActionGroup *mouseModeActionGroup = new QActionGroup(this);
    m_MoveMousePointerDefaultAction = new QAction(tr("&Move Mode"), this);
    m_MoveMousePointerDefaultAction->setCheckable(true);
    m_DrawSignalSlotConnectionActivationArrowsAction = new QAction(tr("&Signals/Slots Connection Activation Arrows Mode"), this);
    m_DrawSignalSlotConnectionActivationArrowsAction->setCheckable(true);
    mouseModeActionGroup->addAction(m_MoveMousePointerDefaultAction);
    mouseModeActionGroup->addAction(m_DrawSignalSlotConnectionActivationArrowsAction);
    connect(m_MoveMousePointerDefaultAction, SIGNAL(triggered()), this, SLOT(doMouseModeChange()));
    connect(m_DrawSignalSlotConnectionActivationArrowsAction, SIGNAL(triggered()), this, SLOT(doMouseModeChange()));
    m_MoveMousePointerDefaultAction->setChecked(true);

    decorateActions();
}
void DesignEqualsImplementationMainWindow::createMenu()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QMenu *fileNew = fileMenu->addMenu(tr("&New"));
    fileNew->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    fileNew->addAction(m_NewProjectAction);
    fileNew->addAction(m_SaveProjectAction);
    fileNew->addAction(m_NewUseCaseAction);
    fileMenu->addAction(m_OpenProjectAction);
    fileMenu->addAction(m_RenameProjectAction);
    fileMenu->addAction(m_GenerateSourceCodeAction);

    fileMenu->addSeparator();

    QAction *quitAction = new QAction(tr("&Quit"), this); //TODOreq: if unsaved changes, are you sure + save and quit blah. same thing should happen when x is pressed or the user shuts down comp
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    fileMenu->addAction(quitAction);

    QMenu *modeMenu = menuBar()->addMenu(tr("&Mouse Mode"));
    modeMenu->addAction(m_MoveMousePointerDefaultAction);
    modeMenu->addAction(m_DrawSignalSlotConnectionActivationArrowsAction);
}
void DesignEqualsImplementationMainWindow::createToolbars()
{
    QToolBar *projectOperationsToolbar_aka_fileNewMenuEquivalentToolbar = addToolBar(tr("Project Operations Toolbar"));
    projectOperationsToolbar_aka_fileNewMenuEquivalentToolbar->addAction(m_NewProjectAction);
    projectOperationsToolbar_aka_fileNewMenuEquivalentToolbar->addAction(m_SaveProjectAction);
    projectOperationsToolbar_aka_fileNewMenuEquivalentToolbar->addAction(m_OpenProjectAction);
    projectOperationsToolbar_aka_fileNewMenuEquivalentToolbar->addAction(m_NewUseCaseAction);
    projectOperationsToolbar_aka_fileNewMenuEquivalentToolbar->addAction(m_GenerateSourceCodeAction);

    //addToolBarBreak();

    QToolBar *mouseModeToolbar = addToolBar(tr("Mouse Mode Toolbar"));
    mouseModeToolbar->addAction(m_MoveMousePointerDefaultAction);
    mouseModeToolbar->addAction(m_DrawSignalSlotConnectionActivationArrowsAction);
}
void DesignEqualsImplementationMainWindow::createDockWidgets()
{
    setCorner(Qt::TopLeftCorner, Qt::TopDockWidgetArea);
    setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks);

    QDockWidget *availableItemsDockWidget = new QDockWidget(tr("Available Items"), this);
    availableItemsDockWidget->setFeatures(DesignEqualsImplementationMainWindow_DOCK_WIDGET_FEATURES);

    m_UmlStackedDockWidget = new QStackedWidget();
    m_ClassDiagramUmlItemsWidget = new ClassDiagramUmlItemsWidget();
    m_UseCaseUmlItemsWidget = new UseCaseUmlItemsWidget();
    m_UmlStackedDockWidget->addWidget(m_ClassDiagramUmlItemsWidget);
    m_UmlStackedDockWidget->addWidget(m_UseCaseUmlItemsWidget);

    availableItemsDockWidget->setWidget(m_UmlStackedDockWidget);
    addDockWidget(Qt::LeftDockWidgetArea, availableItemsDockWidget, Qt::Vertical);

    m_AllUseCasesListWidget = new QListWidget(); //TODOoptional: right-click -> new use case (TODOoptional: semi-OT: class diagram scene right-click -> new class)
    QDockWidget *allUseCasesDockWidget = new QDockWidget(tr("All Use Cases"), this);
    allUseCasesDockWidget->setFeatures(DesignEqualsImplementationMainWindow_DOCK_WIDGET_FEATURES);
    allUseCasesDockWidget->setWidget(m_AllUseCasesListWidget);
    addDockWidget(Qt::LeftDockWidgetArea, allUseCasesDockWidget, Qt::Vertical);

    //defaults
    setClassDiagramToolsDisabled(false);
    setClassDiagramToolsDisabled(true);
}
void DesignEqualsImplementationMainWindow::decorateActions()
{
    m_NewProjectAction->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    m_SaveProjectAction->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    m_OpenProjectAction->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
}
void DesignEqualsImplementationMainWindow::setClassDiagramToolsDisabled(bool disabled)
{
    //an inheritence arrow drawer (that same association should also be specifiable in the "class editor" widget... makes no difference)
}
void DesignEqualsImplementationMainWindow::setUseCaseToolsDisabled(bool disabled)
{
    //TODOoptional: a run-time option/setting could be whether or not to setDisabled or setHidden for all these toolbars/actions... but the default should be setDisabled

    if(disabled && m_DrawSignalSlotConnectionActivationArrowsAction->isChecked())
        m_MoveMousePointerDefaultAction->setChecked(true);
    m_DrawSignalSlotConnectionActivationArrowsAction->setDisabled(disabled);
}
void DesignEqualsImplementationMainWindow::addUseCaseToAllUseCasesListWidget(DesignEqualsImplementationUseCase *newUseCase)
{
    QListWidgetItem *newUseCaseListWidgetItem = new QListWidgetItem(newUseCase->Name);
    newUseCaseListWidgetItem->setData(Qt::UserRole, QVariant::fromValue<DesignEqualsImplementationUseCase*>(newUseCase));
    m_AllUseCasesListWidget->addItem(newUseCaseListWidgetItem);
}
void DesignEqualsImplementationMainWindow::handleE(const QString &msg)
{
    QMessageBox::critical(this, tr("Error"), msg); //TODOoptional: an expanding-when-relevant pane at the bottom, to better handle multiple errors
}
//TODOreq: [backend] project is not thread safe to access directly, so check the source to make sure it's used properly (or i could mutex protect the DesignEqualsImplementationProject itself to KISS, undecided as of now)
void DesignEqualsImplementationMainWindow::handleProjectOpened(DesignEqualsImplementationProject *project)
{
    connect(project, SIGNAL(classAdded(DesignEqualsImplementationClass*)), m_UseCaseUmlItemsWidget, SLOT(handleClassAdded(DesignEqualsImplementationClass*))); //Impulsively I feel the need to sever these connections when project tabs change, but since the backend project can't add a class when it's not current tab, there is little need (memory optimization? idfk)
    connect(project, SIGNAL(useCaseAdded(DesignEqualsImplementationUseCase*)), this, SLOT(handleUseCaseAdded(DesignEqualsImplementationUseCase*)));
    DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget *designEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget = new DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget(project);
    connect(this, SIGNAL(mouseModeChanged(DesignEqualsImplementationMouseModeEnum)), designEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget, SIGNAL(mouseModeChanged(DesignEqualsImplementationMouseModeEnum))); //shouldn't the setCurrentIndex trigger the tab changed slot, which re-does this? OH the reason I didn't see it is because m_CurrentProjectTabIndex starts at 0 and the first added is also 0. BUT i tried changing m_CurrentProjectTabIndex to -1 in this constructor, but then I saw 2x "Foo" instead of Actor + Foo, so I'm leik wat and fuck it this is good enough...
    int tabIndex = m_OpenProjectsTabWidget->addTab(designEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget, project->Name);
    connect(designEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget->classDiagramAndUseCasesTabWidget(), SIGNAL(currentChanged(int)), this, SLOT(handleProjectTabWidgetOrClassDiagramAndUseCasesTabWidgetCurrentTabChanged()));
    m_OpenProjectsTabWidget->setCurrentIndex(tabIndex);
}
void DesignEqualsImplementationMainWindow::handleSaveRequested()
{
    //TODOreq

    DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget *currentProjectTab = static_cast<DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget*>(m_OpenProjectsTabWidget->currentWidget()/*m_OpenProjectsTabWidget->widget(m_CurrentProjectTabIndex*/);

    QString dest("/run/shm/designEqualsImplementation-ProjectSavedAt_" + QString::number(QDateTime::currentMSecsSinceEpoch())); //TODOreq: cryptographic hash of file contents (placeholder to be filled in once calculated(?)) in filename, in addition to the time of course. SINCE we are using the cryptographic hash, the milliseconds/etc elements of the timestamp aren't needed (at least, not to avoid file name collision)
    emit saveProjectRequested(currentProjectTab->designEqualsImplementationProject(), dest);
}
void DesignEqualsImplementationMainWindow::handleOpenProjectActionTriggered()
{
    //TODOreq: file dialog and then
    QFileDialog openFileDialog(this, tr("Open project..."));
    openFileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    if(openFileDialog.exec() == QDialog::Accepted && openFileDialog.selectedFiles().size() > 0)
    {
        emit openExistingProjectRequested(openFileDialog.selectedFiles().at(0));
    }
}
#if !(QT_VERSION < QT_VERSION_CHECK(5, 2, 0))
void DesignEqualsImplementationMainWindow::handleProjectTabDoubleClicked(int indexOfTab)
{
    if(indexOfTab < 0) //they double-clicked the tab BAR, but not any tab in particular
        return;
    handleRenameProjectActionTriggered(); //relies on currentTab. can you even double click on a tab without making it current lol?
}
#endif
void DesignEqualsImplementationMainWindow::handleRenameProjectActionTriggered()
{
    DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget *currentProjectTab = static_cast<DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget*>(m_OpenProjectsTabWidget->currentWidget());

    RenameProjectDialog dialog(currentProjectTab->designEqualsImplementationProject(), this);
    dialog.exec();
}
void DesignEqualsImplementationMainWindow::handleNewUseCaseActionTriggered()
{
    DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget *currentProjectTab = static_cast<DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget*>(m_OpenProjectsTabWidget->currentWidget());
    QMetaObject::invokeMethod(currentProjectTab, "requestNewUseCase");
}
void DesignEqualsImplementationMainWindow::handleProjectTabWidgetOrClassDiagramAndUseCasesTabWidgetCurrentTabChanged()
{
    //NOTE: the reason we don't use the "new tab index" arg from the emitting signalS is because both the "project tabs" changing and the "uml and use cases tabs" changing trigger this slot. not a huge deal since all we're doing is determining which set of tools to present (class diagram vs. use case).

    int newProjectTabIndexMaybe = m_OpenProjectsTabWidget->currentIndex(); //maybe because it may not have changed
    DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget *designEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget = static_cast<DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget*>(m_OpenProjectsTabWidget->widget(newProjectTabIndexMaybe));
    int projectTabSubIndex = designEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget->classDiagramAndUseCasesTabWidget()->currentIndex();

    if(m_CurrentProjectTabIndex == newProjectTabIndexMaybe)
    {
        if(projectTabSubIndex == 0)
        {
            //current project simply changed to class diagram tab (do nothing, rasing it to the stack widget is handled below... and it's contents are already correct)
        }
        else
        {
            //current project changed to use case tab (do nothing?)
        }
    }
    else
    {
        //project tab changed! definitely need to update uml items

        disconnect(this, SIGNAL(mouseModeChanged(DesignEqualsImplementationMouseModeEnum)));
        connect(this, SIGNAL(mouseModeChanged(DesignEqualsImplementationMouseModeEnum)), designEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget, SIGNAL(mouseModeChanged(DesignEqualsImplementationMouseModeEnum)));

        //[re-]populate all use cases list
        m_AllUseCasesListWidget->clear();
        //Q_FOREACH()
        //TODOreq: invokeMethod(project, "emitAllUseCases") vs. lock+iterate. BOTH require locking anyways haha, so ok I'm going to answer the question AS IF I ALREADY HAD IMPLICIT SHARING IMPLEMENTED. *thinks about a hypothetical situation that does not yet exist* (all hypothetical situations do exist, if they are possible, because there is infinite time (depends how you define 'exist'... does it mean: 'known to exist in this [known] universe and has happend (has existed) sometime in between the big bang and now'? or: 'has ever existed'? I would argue the latter, therefore everything exists (if you restrict what can be a thing to anything possible in the realm of physics)))
        //back on track, implicit sharing: hmm yea i'd iterate that list like a fucking baller
        DesignEqualsImplementationProject *project = designEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget->designEqualsImplementationProject();
        Q_FOREACH(DesignEqualsImplementationUseCase *currentUseCase, project->useCases())
        {
            addUseCaseToAllUseCasesListWidget(currentUseCase);
        }

        //[re-]populate all classes in "use case uml items widget"
        QMetaObject::invokeMethod(m_UseCaseUmlItemsWidget, "handleNowShowingProject", Q_ARG(DesignEqualsImplementationProject*, project)); //TODOoptional: sexy signal instead? fuckit. maybe i should separate the tab changed listens after all...

        if(projectTabSubIndex == 0)
        {
            //project tab is/was on class diagram tab
        }
        else
        {
            //project tab is/was on a use case tab
        }
    }

    //TODOreq: don't 'double show'. as in, don't change tools if the correct ones are already being shown

    //If I ever allow tab re-ordering and class diagram doesn't doesn't have tab index == 0, i can still easily check what kind of tab is the new current by doing a qobject_cast ;-P
    if(projectTabSubIndex == 0)
    {
        //show class diagram tools
        setClassDiagramToolsDisabled(false);
        setUseCaseToolsDisabled(true);
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

    m_CurrentProjectTabIndex = newProjectTabIndexMaybe;
    doMouseModeChange(); //update possibly stale values
}
void DesignEqualsImplementationMainWindow::handleUseCaseAdded(DesignEqualsImplementationUseCase *newUseCase)
{
    addUseCaseToAllUseCasesListWidget(newUseCase);
}
void DesignEqualsImplementationMainWindow::handleAllUseCasesListWidgetItemDoubleClicked(QListWidgetItem *doubleClickedListWidgetItem)
{
    DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget *projectTab = static_cast<DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget*>(m_OpenProjectsTabWidget->currentWidget());
    projectTab->ensureUseCaseTabOpenedAkaCreatedAndMakeCurrentWidget(qvariant_cast<DesignEqualsImplementationUseCase*>(doubleClickedListWidgetItem->data(Qt::UserRole)));
}
void DesignEqualsImplementationMainWindow::doMouseModeChange()
{
    if(m_MoveMousePointerDefaultAction->isChecked())
    {
        emit mouseModeChanged(DesignEqualsImplementationMouseMoveMode);
    }
    else if(m_DrawSignalSlotConnectionActivationArrowsAction->isChecked())
    {
        emit mouseModeChanged(DesignEqualsImplementationMouseDrawSignalSlotConnectionActivationArrowsMode);
    }
}
void DesignEqualsImplementationMainWindow::handleGenerateSourceCodeActionTriggered()
{
    //TODOreq: dialog asking them what kind of source to generate, plus a destination folder

    //hack/temp
    QString dest("/run/shm/designEqualsImplementation-GeneratedAt_" + QString::number(QDateTime::currentMSecsSinceEpoch()));
    QMetaObject::invokeMethod(static_cast<DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget*>(m_OpenProjectsTabWidget->currentWidget())->designEqualsImplementationProject(), "generateSourceCode", Q_ARG(DesignEqualsImplementationProject::ProjectGenerationMode, DesignEqualsImplementationProject::Library), Q_ARG(QString,dest));
}
