#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSASQGRAPHICSITEMFORCLASSDIAGRAMSCENE_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSASQGRAPHICSITEMFORCLASSDIAGRAMSCENE_H

#include <QGraphicsWidget>
#include <QPen>

#include "../../designequalsimplementationclass.h"

class DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene(DesignEqualsImplementationClass *designEqualsImplementationClass, QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
private:
    DesignEqualsImplementationClass *m_DesignEqualsImplementationClass;
    QRectF m_BoundingRect;
    QPen m_ClassBorderPen;
    QPen m_LinesInBetweenLinesOfTextPen;
public slots:
    void handlePropertyAdded(DesignEqualsImplementationClassProperty*);
    void handleHasAPrivateMemberClassAdded(HasA_Private_Classes_Members_ListEntryType);
    void handlePrivateMethodAdded(DesignEqualsImplementationClassPrivateMethod*);
    void handleSlotAdded(DesignEqualsImplementationClassSlot*);
    void handleSignalAdded(DesignEqualsImplementationClassSignal*);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSASQGRAPHICSITEMFORCLASSDIAGRAMSCENE_H
