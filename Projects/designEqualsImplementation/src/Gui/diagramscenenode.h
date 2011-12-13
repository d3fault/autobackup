#ifndef DIAGRAMSCENENODE_H
#define DIAGRAMSCENENODE_H

#include <QGraphicsPolygonItem>
#include <QColor>
#include <QPixmap>
#include <QPainter>
#include <QPen>

#include "../ClassesThatRepresentProjectBeingCreated/designprojecttemplates.h"

//class DiagramSceneNode;

class DiagramSceneNode : public QGraphicsPolygonItem
{
public:
    DiagramSceneNode(int uniqueId, DesignProjectTemplates::DesignProjectViewType viewType);
    int getUniqueId();
    DesignProjectTemplates::DesignProjectViewType getViewType();
    virtual QString getNodeTypeAsString()=0;
    QPixmap image() const;
    virtual void drawMyPolygon()=0;
    QPolygonF polygon() const
        { return m_MyPolygon; }
    //DiagramSceneNode *copyNode();
    //virtual DiagramSceneNode *cloneSelf();
protected:
    QPolygonF m_MyPolygon;
    int m_UniqueId;
    DesignProjectTemplates::DesignProjectViewType m_ViewType;
};

#endif // DIAGRAMSCENENODE_H
