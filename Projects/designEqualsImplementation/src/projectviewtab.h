#ifndef PROJECTVIEWTAB_H
#define PROJECTVIEWTAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QGraphicsView>

#include <designprojectview.h>
#include <dragdropdiagramscene.h>

class ProjectViewTab : public QWidget
{
    Q_OBJECT
public:
    explicit ProjectViewTab(DesignProjectView *projectView);
    DesignProjectView *getProjectView();
private:
    //gui
    QVBoxLayout *m_Layout;
    QGraphicsView *m_GraphicsView;
    DragDropDiagramScene *m_GraphicsScene;
    //members
    DesignProjectView *m_ProjectView;
signals:

public slots:

};

#endif // PROJECTVIEWTAB_H
