#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_Failed(true) /* true until m_CurrentProject is succesfully extracted/casted */
{
    createActions();
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
void MainWindow::handleNewProjectAction()
{
    //TODOreq: this should be called on first launch, and it should create a new project as well as our default classes... say 1 front end and 1 backend. there is no default use case. they launch it and nothing happens. the backend is NOT started automatically. i guess it could be and could emit some "hello world" or something to our debug pane or something... doesn't really matter
    QString newProjectName("New Project 1");
    ProjectTab *newProjectTab = new ProjectTab(new DesignProject(newProjectName));
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
