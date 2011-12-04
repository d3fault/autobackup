#ifndef DRAGDROPDIAGRAMSCENE_H
#define DRAGDROPDIAGRAMSCENE_H

#include <QGraphicsScene>

#include "diagramscenenode.h"

class DragDropDiagramScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit DragDropDiagramScene(QObject *parent = 0);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
private:
    bool expectingNode();

signals:

public slots:
    void handleModeChanged()
};

#endif // DRAGDROPDIAGRAMSCENE_H
