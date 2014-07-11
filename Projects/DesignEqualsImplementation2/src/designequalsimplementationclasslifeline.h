#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H

#include <QObject>
#include <QPointF>
#include <QList>

class DesignEqualsImplementationClass;
class HasA_Private_Classes_Members_ListEntryType;
class DesignEqualsImplementationClassLifeLineUnitOfExecution;

class DesignEqualsImplementationClassLifeLine : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassLifeLine(DesignEqualsImplementationClass *designEqualsImplementationClass, HasA_Private_Classes_Members_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject, QPointF position, QObject *parent = 0);
    QPointF position() const;
    DesignEqualsImplementationClass *designEqualsImplementationClass() const;
    HasA_Private_Classes_Members_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject() const;
    void insertUnitOfExecution(int indexToInsertInto, DesignEqualsImplementationClassLifeLineUnitOfExecution *newUnitOfExecution);
    QList<DesignEqualsImplementationClassLifeLineUnitOfExecution *> unitsOfExecution() const;
private:
    DesignEqualsImplementationClass *m_DesignEqualsImplementationClass;
    HasA_Private_Classes_Members_ListEntryType *m_MyInstanceInClassThatHasMe_OrZeroIfTopLevelObject;
    QPointF m_Position;
    QList<DesignEqualsImplementationClassLifeLineUnitOfExecution*> m_UnitsOfExecution;
signals:
    void unitOfExecutionInserted(int indexInsertedInto, DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H
