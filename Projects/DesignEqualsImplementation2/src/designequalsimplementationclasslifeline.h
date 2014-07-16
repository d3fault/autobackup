#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H

#include <QObject>
#include <QPointF>
#include <QList>
#include <QMap>

class DesignEqualsImplementationClass;
class HasA_Private_Classes_Members_ListEntryType;
class DesignEqualsImplementationClassSlot;
//class DesignEqualsImplementationClassLifeLineUnitOfExecution;

typedef QMap<DesignEqualsImplementationClassSlot*, int> MySlotsAppearingInClassLifeLineRefcountTypedef;

class DesignEqualsImplementationClassLifeLine : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassLifeLine(DesignEqualsImplementationClass *designEqualsImplementationClass, HasA_Private_Classes_Members_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject, QPointF position, QObject *parent = 0);
    //explicit DesignEqualsImplementationClassLifeLine(DesignEqualsImplementationClass *designEqualsImplementationClass, DesignEqualsImplementationClassSlot *firstSlot, HasA_Private_Classes_Members_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject, QPointF position, QObject *parent = 0);
    QPointF position() const;
    DesignEqualsImplementationClass *designEqualsImplementationClass() const;
    HasA_Private_Classes_Members_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject() const;
    void insertSlotReference(DesignEqualsImplementationClassSlot *newSlot);
    void clearMySlotReferences();
    //void replaceSlot(int indexToReplace, DesignEqualsImplementationClassSlot *slotToReplaceItWith);
    MySlotsAppearingInClassLifeLineRefcountTypedef mySlots() const;
private:
    //friend class DesignEqualsImplementationClassLifeLineUnitOfExecution;
    DesignEqualsImplementationClass *m_DesignEqualsImplementationClass;
    HasA_Private_Classes_Members_ListEntryType *m_MyInstanceInClassThatHasMe_OrZeroIfTopLevelObject;
    QPointF m_Position;
    MySlotsAppearingInClassLifeLineRefcountTypedef m_MySlotsAppearingInClassLifeLine_References;
signals:
    void slotReferencedInClassLifeLine(DesignEqualsImplementationClassSlot *slot);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H
