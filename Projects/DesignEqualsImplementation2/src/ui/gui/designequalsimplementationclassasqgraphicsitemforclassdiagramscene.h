#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSASQGRAPHICSITEMFORCLASSDIAGRAMSCENE_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSASQGRAPHICSITEMFORCLASSDIAGRAMSCENE_H

#include <QGraphicsRectItem>
#include <QObject>
#include <QPen>

#include "../../type.h"

class QGraphicsSceneContextMenuEvent;
class QGraphicsSceneMouseEvent;

class DesignEqualsImplementationProject;

class DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene(Type *designEqualsImplementationClass, DesignEqualsImplementationProject *currentProject, QGraphicsItem *graphicsParent = 0, QObject *qobjectParent = 0);
    //virtual QRectF boundingRect() const;
    //virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
private:
    QGraphicsTextItem *m_ClassContentsGraphicsTextItem;
    Type *m_Type;
    DesignEqualsImplementationProject *m_CurrentProject;
    QRectF m_BoundingRect;
    QPen m_ClassBorderPen;
    QPen m_LinesInBetweenLinesOfTextPen;

    QString classDetailsAsHtmlString();
    void updateClassContentsGraphicsTextItem();
signals:
    void editCppModeRequested(Type *type);
public slots:
    void handleNonFunctionMemberAdded(NonFunctionMember *propertyAdded);
    void handlePrivateMethodAdded(DesignEqualsImplementationClassPrivateMethod*);
    void handleSlotAdded(DesignEqualsImplementationClassSlot *slotAdded);
    void handleSignalAdded(DesignEqualsImplementationClassSignal*);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSASQGRAPHICSITEMFORCLASSDIAGRAMSCENE_H
