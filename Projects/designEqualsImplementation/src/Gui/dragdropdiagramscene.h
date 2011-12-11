#ifndef DRAGDROPDIAGRAMSCENE_H
#define DRAGDROPDIAGRAMSCENE_H

#include <QGraphicsScene>

#include "diagramscenenode.h"
#include "../StateMachine/modesingleton.h"
#include <QGraphicsSceneMouseEvent>

class DragDropDiagramScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit DragDropDiagramScene(QObject *parent = 0);
private:
    bool m_ExpectingNode;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
public slots:
    void handleModeChanged(ModeSingleton::Mode);
};

#endif // DRAGDROPDIAGRAMSCENE_H
