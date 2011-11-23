#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_Failed(true) /* true until m_CurrentProject is succesfully extracted/casted */
{
    createActions();
    createMenus();
    createLeftPane();
    createProjectTabWidget();
    //gui
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(m_LeftPane);
    layout->addWidget(m_ProjectTabWidgetContainer);
    //m_GraphicsView = new QGraphicsView(m_GraphicsScene);
    //layout->addWidget(m_GraphicsView);

    QWidget *widget = new QWidget();
    widget->setLayout(layout);

    this->setCentralWidget(widget);
    this->setWindowTitle(tr("Design = Implementation"));
    setUnifiedTitleAndToolBarOnMac(true);

    //connections
    connect(m_ProjectTabWidgetContainer, SIGNAL(currentChanged(int)), this, SLOT(handleProjectTabChanged(int)));


    //send a message to ourselves when we first launch the application to open up a blank new project
    //TODOopt: if we have a project as our argument, open that one instead and don't create a blank new project
    QMetaObject::invokeMethod(this, "handleNewProjectAction");
}
MainWindow::~MainWindow()
{

}
void MainWindow::createActions()
{
    m_NewProjectAction = new QAction(tr("&Project"), this);
    m_NewUseCaseAction = new QAction(tr("&Use Case"), this);

    connect(m_NewProjectAction, SIGNAL(triggered()), this, SLOT(handleNewProjectAction()));
    connect(m_NewUseCaseAction, SIGNAL(triggered()), this, SLOT(handleNewUseCaseAction()));
}
void MainWindow::createMenus()
{
    QMenu *fileMenu = this->menuBar()->addMenu(tr("File"));
    QMenu *fileNewMenu = fileMenu->addMenu(tr("New"));
    fileNewMenu->addAction(m_NewProjectAction);
    fileNewMenu->addAction(m_NewUseCaseAction);
}
void MainWindow::handleNewProjectAction()
{
    //TODOreq: this should be called on first launch, and it should create a new project as well as our default classes... say 1 front end and 1 backend. there is no default use case. they launch it and nothing happens. the backend is NOT started automatically. i guess it could be and could emit some "hello world" or something to our debug pane or something... doesn't really matter
    QString newProjectName("New Project 1");
    DesignProject *newDesignProject = new DesignProject(newProjectName);
    ProjectTab *newProjectTab = new ProjectTab(newDesignProject);
    newDesignProject->createEmptyProject(); //when loading instead of creating a new project, this call would be different. existingDesignProject->loadClassDiagramAndUseCases(&from etc idfk); the "existing" in existing project tab is still a new tab in memory... but the project is loaded from a saved file. the project is already existing, the tab is now (until we add it like just below)
    //^^if the above comment makes no sense, it's because i changed it from "tab" to just "object" later on... and from emptyClassDiagram to emptyProject. better design.
    //TODOopt: right now if i call createEmptyProject before doing the new ProjectTab, the tab won't show up. they are only added by listening to DesignProject newProjectView signal... but the instantiator for ProjectTab should also search existing Views and add tabs accordingly
    connect(newProjectTab, SIGNAL(e(const QString &)), this, SIGNAL(e(const QString &)));
    int newTabIndex = m_ProjectTabWidgetContainer->addTab(newProjectTab, newProjectName);
    m_ProjectTabWidgetContainer->setCurrentIndex(newTabIndex);
    //TODOreq: "1" above should be 2, 3, etc depending on if "New Project 1" is already open etc
}
void MainWindow::handleNewUseCaseAction()
{
    //TODOreq: adds a new use case (and switches to use case view mode for the new use case) to the current project
}
void MainWindow::createLeftPane()
{
    //TODOreq: a vbox layout. the upper half has a two-tabbed thingy. one tab = class diagram mode diagrams, the other = use case mode diagrams. toggling between the two changes the graphics view currently open to non-editable(grayed out) if the modes don't match. respecitvely, changing the tabs from "overview mode" to any of the use cases that are currently open (or if opened from the bottom half of THIS vbox layout) changes the tab to it's corresponding mode.
    //TODOreq: the bottom half contains all of the views for the current project. the top one is always the Overview (class diagram mode). as mentioned in the comment directly above, selecting one opens it in a tab (or changes to that tab if it's already opened) and sets the upper half of this vbox layout's diagram add groupbox tab shits to display the corresponding group of diagram objects that can be added
    m_LeftPane = new QWidget();
    QVBoxLayout *leftPaneLayout = new QVBoxLayout();
    QTabWidget *useCaseAndClassDiagramViewsNodesTemplateSelectorButtonGroupTabWidget = new QTabWidget();
    createNodeButtonGroups();
    leftPaneLayout->addWidget(useCaseAndClassDiagramViewsNodesTemplateSelectorButtonGroupTabWidget);
    //createListForClassDiagramAndAllUseCases();
    //leftPaneLayout->addWidget(listOfUseCasesForCurrentProjectOhAndAlsoClassDiagramAtTheTopKthx);

    m_LeftPane->setLayout(leftPaneLayout);
}
void MainWindow::createNodeButtonGroups()
{
    //TODOreq: left off here, re-arranged DesignProject... but i want some way to iterate through ProjectViewTypes to make 2("x") tabs here, and another way to iterate through each nodeType for each viewType to create the buttonGroup (2 colums, x rows (depending how many)) for each viewType/tab
    DesignProjectTemplates *dpt = new DesignProjectTemplates();
    QList<DesignProjectTemplates::DesignProjectViewType> i = dpt->AllDesignProjectNodesByProjectViewType->uniqueKeys();
    int viewTypesLength = i.length();
    for(int j = 0; j < viewTypesLength; ++j)
    {
        //add tab for this DesignProjectViewType (class diagram, use case, etc)
        QList<DiagramSceneNode*> dpt->AllDesignProjectNodesByProjectViewType->values(i.at(j));
        //add a button to the groupbox for this tab.. maybe do other stuff (look at that handy example)... connect to it's signals/slots etc?

    }


    //class diagram button group
    m_ClassDiagramNodeButtonGroup = new QButtonGroup();
    m_ClassDiagramNodeButtonGroup->setExclusive(false);
    connect(m_ClassDiagramNodeButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(handleButtonGroupButtonClicked(int)));

    //use case button group

}
void MainWindow::createProjectTabWidget()
{
    m_ProjectTabWidgetContainer = new QTabWidget();
}
void MainWindow::handleProjectTabChanged(int index)
{
    m_CurrentProject = qobject_cast<ProjectTab*>(m_ProjectTabWidgetContainer->widget(index))->getProject();
    if(!m_CurrentProject)
    {
        emit e("Project Tab Cast Failed");
        m_Failed = true;
        return;
    }
    m_Failed = false;

    //after this, nothing really... maybe redraw the qgraphicsscene based on the contents of the [new] current project...
    //...but mainly this is only necessary for later drag and drops / oonnections made
}
void MainWindow::handleButtonGroupButtonClicked(int)
{
    if(mode == classDiagramMode)
    {

    }
    else if(mode == useCaseDiagramMode)
    {

    }
    //NOTE, the above if/else might not be necessary at all until the actual click onto the graphics scene...
    //the point is just to show that they share a handler
}
