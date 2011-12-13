#ifndef DRAGDROPDIAGRAMSCENE_H
#define DRAGDROPDIAGRAMSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include "diagramscenenode.h"
#include "../StateMachine/modesingleton.h"
#include "../ClassesThatRepresentProjectBeingCreated/designprojectview.h"

class DesignProject;

class DragDropDiagramScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit DragDropDiagramScene(DesignProjectView *projectView);
private:
    bool m_ExpectingNode;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
public slots:
    void handleModeChanged(ModeSingleton::Mode);
private slots:
    void handleNodeAdded(DiagramSceneNode *node);
};

#endif // DRAGDROPDIAGRAMSCENE_H
