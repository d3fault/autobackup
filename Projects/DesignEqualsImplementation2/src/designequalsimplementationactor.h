#ifndef DESIGNEQUALSIMPLEMENTATIONACTOR_H
#define DESIGNEQUALSIMPLEMENTATIONACTOR_H

#include <QObject>
#include <QPointF>

class DesignEqualsImplementationActor : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationActor(QPointF position, QObject *parent = 0);
    QPointF position() const;
private:
    QPointF m_Position;
};

#endif // DESIGNEQUALSIMPLEMENTATIONACTOR_H
