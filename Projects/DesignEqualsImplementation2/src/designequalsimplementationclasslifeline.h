#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H

#include <QObject>
#include <QPointF>
#include <QList>

class DesignEqualsImplementationClass;
class HasA_Private_Classes_Members_ListEntryType;
class DesignEqualsImplementationClassSlot;
//class DesignEqualsImplementationClassLifeLineUnitOfExecution;

class DesignEqualsImplementationClassLifeLine : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassLifeLine(DesignEqualsImplementationClass *designEqualsImplementationClass, HasA_Private_Classes_Members_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject, QPointF position, QObject *parent = 0);
    explicit DesignEqualsImplementationClassLifeLine(DesignEqualsImplementationClass *designEqualsImplementationClass, DesignEqualsImplementationClassSlot *firstSlot, HasA_Private_Classes_Members_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject, QPointF position, QObject *parent = 0);
    QPointF position() const;
    DesignEqualsImplementationClass *designEqualsImplementationClass() const;
    HasA_Private_Classes_Members_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject() const;
    void insertSlot(int indexToInsertInto, DesignEqualsImplementationClassSlot *newSlot);
    //void replaceSlot(int indexToReplace, DesignEqualsImplementationClassSlot *slotToReplaceItWith);
    QList<DesignEqualsImplementationClassSlot*> mySlots() const;
private:
    //friend class DesignEqualsImplementationClassLifeLineUnitOfExecution;
    DesignEqualsImplementationClass *m_DesignEqualsImplementationClass;
    HasA_Private_Classes_Members_ListEntryType *m_MyInstanceInClassThatHasMe_OrZeroIfTopLevelObject;
    QPointF m_Position;
    QList<DesignEqualsImplementationClassSlot*> m_MySlots;
signals:
    void slotInserted(int indexInsertedInto, DesignEqualsImplementationClassSlot *slot);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H
