#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H

#include <QObject>
#include <QPointF>
#include <QList>

class DesignEqualsImplementationClass;
class DesignEqualsImplementationClassLifeLineUnitOfExecution;

class DesignEqualsImplementationClassLifeLine : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassLifeLine(DesignEqualsImplementationClass *designEqualsImplementationClass, QPointF position, QObject *parent = 0);
    QPointF position() const;
    DesignEqualsImplementationClass *designEqualsImplementationClass() const;
    QList<DesignEqualsImplementationClassLifeLineUnitOfExecution *> unitsOfExecution() const;
private:
    DesignEqualsImplementationClass *m_DesignEqualsImplementationClass;
    QPointF m_Position;
    QList<DesignEqualsImplementationClassLifeLineUnitOfExecution*> m_UnitsOfExecution;
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H
