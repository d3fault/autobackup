#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H

#include <QObject>
#include <QPointF>
#include <QList>
#include <QMap>

class DesignEqualsImplementationClass;
class DesignEqualsImplementationClassInstance;
class DesignEqualsImplementationClassSlot;
//class DesignEqualsImplementationClassLifeLineUnitOfExecution;

typedef QMap<DesignEqualsImplementationClassSlot*, int> MySlotsAppearingInClassLifeLineRefcountTypedef;

class DesignEqualsImplementationClassLifeLine : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassLifeLine(DesignEqualsImplementationClass *designEqualsImplementationClass, DesignEqualsImplementationClassInstance *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject, QPointF position, QObject *parent = 0);
    //explicit DesignEqualsImplementationClassLifeLine(DesignEqualsImplementationClass *designEqualsImplementationClass, DesignEqualsImplementationClassSlot *firstSlot, HasA_Private_Classes_Members_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject, QPointF position, QObject *parent = 0);
    QPointF position() const;
    DesignEqualsImplementationClass *designEqualsImplementationClass() const;
    void setMyInstanceInClassThatHasMe_OrZeroIfTopLevelObject(DesignEqualsImplementationClassInstance *hasA_Private_Classes_Members_ListEntryType);
    DesignEqualsImplementationClassInstance *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject() const;
    void insertSlotReference(DesignEqualsImplementationClassSlot *newSlot);
    void removeSlotReference(DesignEqualsImplementationClassSlot *slot);
    void clearMySlotReferences();
    //void replaceSlot(int indexToReplace, DesignEqualsImplementationClassSlot *slotToReplaceItWith);
    MySlotsAppearingInClassLifeLineRefcountTypedef mySlots() const;
private:
    //friend class DesignEqualsImplementationClassLifeLineUnitOfExecution;
    DesignEqualsImplementationClass *m_DesignEqualsImplementationClass;
    DesignEqualsImplementationClassInstance *m_MyInstanceInClassThatHasMe;
    QPointF m_Position;
    MySlotsAppearingInClassLifeLineRefcountTypedef m_MySlotsAppearingInClassLifeLine_References;
signals:
    void slotReferencedInClassLifeLine(DesignEqualsImplementationClassSlot *slot);
    void slotRemovedFromClassLifeLine(DesignEqualsImplementationClassSlot *slotRemoved);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H
