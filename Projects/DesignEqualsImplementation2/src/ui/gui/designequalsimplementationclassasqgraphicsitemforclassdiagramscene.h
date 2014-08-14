#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSASQGRAPHICSITEMFORCLASSDIAGRAMSCENE_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSASQGRAPHICSITEMFORCLASSDIAGRAMSCENE_H

#include <QGraphicsRectItem>
#include <QObject>
#include <QPen>

#include "../../designequalsimplementationclass.h"

class QGraphicsSceneContextMenuEvent;
class QGraphicsSceneMouseEvent;

class DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene(DesignEqualsImplementationClass *designEqualsImplementationClass, QGraphicsItem *graphicsParent = 0, QObject *qobjectParent = 0);
    //virtual QRectF boundingRect() const;
    //virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
private:
    QGraphicsTextItem *m_ClassContentsGraphicsTextItem;
    DesignEqualsImplementationClass *m_DesignEqualsImplementationClass;
    QRectF m_BoundingRect;
    QPen m_ClassBorderPen;
    QPen m_LinesInBetweenLinesOfTextPen;

    QString classDetailsAsHtmlString();
    void updateClassContentsGraphicsTextItem();
public slots:
    void handlePropertyAdded(DesignEqualsImplementationClassProperty *propertyAdded);
    void handleHasAPrivateMemberClassAdded(HasA_Private_Classes_Member*);
    void handlePrivateMethodAdded(DesignEqualsImplementationClassPrivateMethod*);
    void handleSlotAdded(DesignEqualsImplementationClassSlot*);
    void handleSignalAdded(DesignEqualsImplementationClassSignal*);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSASQGRAPHICSITEMFORCLASSDIAGRAMSCENE_H
