#include "projectviewtab.h"

ProjectViewTab::ProjectViewTab(DesignProjectView *projectView)
{
    //save args
    m_ProjectView = projectView;

    //gui
    m_Layout = new QVBoxLayout();
    m_GraphicsScene = new DragDropDiagramScene(projectView); //TODOopt: maybe use a singleton of the graphics scene since we'll only show one at a time? idk
    m_GraphicsScene->setSceneRect(QRectF(0, 0, 5000, 5000));
    m_GraphicsView = new QGraphicsView(m_GraphicsScene);
    m_Layout->addWidget(m_GraphicsView);
    this->setLayout(m_Layout);

    //connect the scene to the mode singleton's signals
    ModeSingleton *modeSingleton = ModeSingleton::Instance();
    connect(modeSingleton, SIGNAL(modeChanged(ModeSingleton::Mode)), m_GraphicsScene, SLOT(handleModeChanged(ModeSingleton::Mode)));

    //scene will just poll ModeSingleton on mousePress instead... fuck it, idk why that signal/slot isn't working :-/
}
DesignProjectView * ProjectViewTab::getProjectView()
{
    return m_ProjectView;
}
