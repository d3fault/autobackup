#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEGRAPHICSITEMFORUSECASESCENE_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEGRAPHICSITEMFORUSECASESCENE_H

#include <QGraphicsWidget>
#include <QPen>

class DesignEqualsImplementationClass;

class DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(DesignEqualsImplementationClass *designEqualsImplementationClass, QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
private:
    DesignEqualsImplementationClass *m_DesignEqualsImplementationClass;
    QRectF m_BoundingRect;
    QPen m_ClassBorderPen;
    QPen m_LifelineNoActivityPen;
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEGRAPHICSITEMFORUSECASESCENE_H
