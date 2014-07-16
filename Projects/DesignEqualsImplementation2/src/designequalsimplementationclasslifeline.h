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

typedef QList<DesignEqualsImplementationClassSlot*> MySlotsAppearingInClassLifeLine_List;

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
    void insertSlotToClassLifeLine(int indexToInsertInto, DesignEqualsImplementationClassSlot *newSlot);
    void removeSlotFromClassLifeLine(DesignEqualsImplementationClassSlot *slotToRemove);
    //void replaceSlot(int indexToReplace, DesignEqualsImplementationClassSlot *slotToReplaceItWith);
    MySlotsAppearingInClassLifeLine_List mySlotsAppearingInClassLifeLine() const;
private:
    //friend class DesignEqualsImplementationClassLifeLineUnitOfExecution;
    DesignEqualsImplementationClass *m_DesignEqualsImplementationClass;
    DesignEqualsImplementationClassInstance *m_MyInstanceInClassThatHasMe;
    QPointF m_Position;
    MySlotsAppearingInClassLifeLine_List m_MySlotsAppearingInClassLifeLine;
signals:
    void slotInsertedIntoClassLifeLine(int insertIndex, DesignEqualsImplementationClassSlot *slot);
    void slotRemovedFromClassLifeLine(DesignEqualsImplementationClassSlot *slotRemoved);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H
