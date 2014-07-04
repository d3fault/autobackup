#ifndef IHAVEAGRAPHICSVIEWANDSCENE_H
#define IHAVEAGRAPHICSVIEWANDSCENE_H

#include <QWidget>

class QGraphicsScene;

class IHaveAGraphicsViewAndScene : public QWidget
{
    Q_OBJECT
public:
    explicit IHaveAGraphicsViewAndScene(QGraphicsScene *graphicsScene, QWidget *parent = 0);
protected:
    QGraphicsScene *m_GraphicsScene;
};

#endif // IHAVEAGRAPHICSVIEWANDSCENE_H
