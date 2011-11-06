#include "projectviewtab.h"

ProjectViewTab::ProjectViewTab(DesignProjectView *projectView)
{
    //save args
    m_ProjectView = projectView;

    //gui
    m_Layout = new QVBoxLayout();
    m_GraphicsScene = new DragDropDiagramScene(); //TODOopt: maybe use a singleton of the graphics scene since we'll only show one at a time? idk
    m_GraphicsScene->setSceneRect(QRectF(0, 0, 5000, 5000));
    m_GraphicsView = new QGraphicsView(m_GraphicsScene);
    m_Layout->addWidget(m_GraphicsView);
    this->setLayout(m_Layout);
}
DesignProjectView * ProjectViewTab::getProjectView()
{
    return m_ProjectView;
}
