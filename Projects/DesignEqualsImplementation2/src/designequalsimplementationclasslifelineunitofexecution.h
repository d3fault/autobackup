#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEUNITOFEXECUTION_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEUNITOFEXECUTION_H

#include <QObject>

class DesignEqualsImplementationClassLifeLine;

class DesignEqualsImplementationClassLifeLineUnitOfExecution : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassLifeLineUnitOfExecution(DesignEqualsImplementationClassLifeLine *designEqualsImplementationClassLifeLine, QObject *parent = 0);
    DesignEqualsImplementationClassLifeLine *designEqualsImplementationClassLifeLine() const;
private:
    DesignEqualsImplementationClassLifeLine *m_DesignEqualsImplementationClassLifeLine;
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEUNITOFEXECUTION_H
