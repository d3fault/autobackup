#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H

#include <QObject>
#include <QPointF>
#include <QList>

class DesignEqualsImplementationClass;
class HasA_PrivateMemberClasses_ListEntryType;
class DesignEqualsImplementationClassLifeLineUnitOfExecution;

class DesignEqualsImplementationClassLifeLine : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassLifeLine(DesignEqualsImplementationClass *designEqualsImplementationClass, HasA_PrivateMemberClasses_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject, QPointF position, QObject *parent = 0);
    QPointF position() const;
    DesignEqualsImplementationClass *designEqualsImplementationClass() const;
    HasA_PrivateMemberClasses_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject() const;
    QList<DesignEqualsImplementationClassLifeLineUnitOfExecution *> unitsOfExecution() const;
private:
    DesignEqualsImplementationClass *m_DesignEqualsImplementationClass;
    HasA_PrivateMemberClasses_ListEntryType *m_MyInstanceInClassThatHasMe_OrZeroIfTopLevelObject;
    QPointF m_Position;
    QList<DesignEqualsImplementationClassLifeLineUnitOfExecution*> m_UnitsOfExecution;
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H
