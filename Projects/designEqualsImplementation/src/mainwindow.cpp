#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createActions();
    createLeftPane();
    createProjectTabWidget();
    //gui
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(m_LeftPane);
    layout->addWidget(m_ProjectTabWidget);
    //m_GraphicsView = new QGraphicsView(m_GraphicsScene);
    //layout->addWidget(m_GraphicsView);

    QWidget *widget = new QWidget();
    widget->setLayout(layout);

    this->setCentralWidget(widget);
    this->setWindowTitle(tr("Design = Implementation"));
    setUnifiedTitleAndToolBarOnMac(true);

    //instantiations
    m_OpenProjectsManager = new OpenProjectsManager();

    //connections
    connect(m_OpenProjectsManager, SIGNAL(projectOpened(DesignProject*)), this, SLOT(handleProjectOpened(DesignProject*)));
    connect(m_ProjectTabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleProjectTabChanged(int)));


    //send a message to ourselves when we first launch the application to open up a blank new project
    //TODOopt: if we have a project as our argument, open that one instead and don't create a blank new project
    QMetaObject::invokeMethod(this, "handleNewProjectAction");
}
MainWindow::~MainWindow()
{

}
void MainWindow::createActions()
{
    m_NewProjectAction = new QAction(tr("&Project"));
    m_NewClassAction = new QAction(tr("&Class"));
    m_NewUseCaseAction = new QAction(tr("&Use Case"));

    connect(m_NewProjectAction, SIGNAL(triggered()), this, SLOT(handleNewProjectAction()));
    //connect(m_NewClassAction, SIGNAL(triggered()), this, SLOT(handleNewClassAction()));
    connect(m_NewUseCaseAction, SIGNAL(triggered()), this, SLOT(handleNewUseCaseAction()));
}
void MainWindow::handleNewProjectAction()
{
    //TODOreq: this should be called on first launch, and it should create a new project as well as our default classes... say 1 front end and 1 backend. there is no default use case. they launch it and nothing happens. the backend is NOT started automatically. i guess it could be and could emit some "hello world" or something to our debug pane or something... doesn't really matter
    m_OpenProjectsManager->add(new DesignProject("New Project 1")); //this call should emit signals that update the gui. add a new top-level tab (the list of open projects), and switch to it
    //TODOreq: "1" above should be 2, 3, etc depending on if "New Project 1" is already open etc
}
#if 0
void MainWindow::handleNewClassAction() //TODOreq: this class seems almost unnecessary, but i kinda want it for my File->New Menu
{
    //TODOreq: class type created (OverviewClass vs. UseCaseClass) depends on if we're in class diagram view or use case mode view
    if(m_CurrentViewMode == ClassDiagramViewMode)
    {
        addOverviewClassToCurrentProject(); //TODOreq: this should add a diagram scene item to our scene... where it puts it is another question (0,0 or right in middle?)... but the point is it needs to tie in with the clickToAdd OverviewClass mechanism that is primarily used
    }
    else if(m_CurrentViewMode == UseCaseViewMode)
    {
        addUseCaseClassToCurrentProject(); //TODOreq: see above
    }
}
#endif
void MainWindow::handleNewUseCaseAction()
{
    //TODOreq: adds a new use case (and switches to use case view mode for the new use case) to the current project
}
void MainWindow::handleProjectOpened(DesignProject *openedProject)
{
    this->setUpdatesEnabled(false);
    QTabWidget *viewModeTabWidget = new QTabWidget();
    OverviewModeGraphicsScene *overviewModeGraphicsScene = new OverviewModeGraphicsScene();
    QGraphicsView *overviewModeGraphicsView = new QGraphicsView();
    overviewModeGraphicsView->setScene(overviewModeGraphicsScene);
    viewModeTabWidget.addTab(overviewModeGraphicsView, "Overview (Class Diagram)");
    m_ProjectTabWidget->addTab(viewModeTabWidget, openedProject->getProjectName()); //TODO: the new QTabWidget here represents all the tabs that are open. the overview tab and any/all open use case views etc

    this->setUpdatesEnabled(true);
}
void MainWindow::createLeftPane()
{
    //TODOreq: a vbox layout. the upper half has a two-tabbed thingy. one tab = class diagram mode diagrams, the other = use case mode diagrams. toggling between the two changes the graphics view currently open to non-editable(grayed out) if the modes don't match. respecitvely, changing the tabs from "overview mode" to any of the use cases that are currently open (or if opened from the bottom half of THIS vbox layout) changes the tab to it's corresponding mode.
    //TODOreq: the bottom half contains all of the views for the current project. the top one is always the Overview (class diagram mode). as mentioned in the comment directly above, selecting one opens it in a tab (or changes to that tab if it's already opened) and sets the upper half of this vbox layout's diagram add groupbox tab shits to display the corresponding group of diagram objects that can be added
}
void MainWindow::createProjectTabWidget()
{
    m_ProjectTabWidget = new QTabWidget();
}
void MainWindow::handleProjectTabChanged(int index)
{
    m_CurrentProject = m_OpenProjectsManager->getByTabIndex(index);
}
