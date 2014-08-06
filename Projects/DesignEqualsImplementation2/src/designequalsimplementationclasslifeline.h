#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H

#include <QObject>
#include <QPointF>
#include <QList>
#include <QMap>

class DesignEqualsImplementationClass;
class DesignEqualsImplementationClassSlot;
//class DesignEqualsImplementationClassLifeLineUnitOfExecution;

typedef QList<DesignEqualsImplementationClassSlot*> MySlotsAppearingInClassLifeLine_List;

class DesignEqualsImplementationClassLifeLine : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassLifeLine(DesignEqualsImplementationClass *designEqualsImplementationClass, /*TODOinstancing: DesignEqualsImplementationClassInstance *myInstanceInClassThatHasMe, */QPointF position, QObject *parent = 0);
    //explicit DesignEqualsImplementationClassLifeLine(DesignEqualsImplementationClass *designEqualsImplementationClass, DesignEqualsImplementationClassSlot *firstSlot, HasA_Private_Classes_Members_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline, QPointF position, QObject *parent = 0);
    QPointF position() const;
    DesignEqualsImplementationClass *designEqualsImplementationClass() const;
#if 0 //TODOinstancing
    void setmyInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline(DesignEqualsImplementationClassInstance *hasA_Private_Classes_Members_ListEntryType);
    DesignEqualsImplementationClassInstance *myInstanceInClassThatHasMe() const;
#endif
    void insertSlotToClassLifeLine(int indexToInsertInto, DesignEqualsImplementationClassSlot *newSlot);
    void removeSlotFromClassLifeLine(DesignEqualsImplementationClassSlot *slotToRemove);
    //void replaceSlot(int indexToReplace, DesignEqualsImplementationClassSlot *slotToReplaceItWith);
    MySlotsAppearingInClassLifeLine_List mySlotsAppearingInClassLifeLine() const;
private:
    //friend class DesignEqualsImplementationClassLifeLineUnitOfExecution;
    DesignEqualsImplementationClass *m_DesignEqualsImplementationClass;
    //TODOinstancing: DesignEqualsImplementationClassInstance *m_MyInstanceInClassThatHasMe;
    QPointF m_Position;
    MySlotsAppearingInClassLifeLine_List m_MySlotsAppearingInClassLifeLine;
signals:
    void slotInsertedIntoClassLifeLine(int insertIndex, DesignEqualsImplementationClassSlot *slot);
    void slotRemovedFromClassLifeLine(DesignEqualsImplementationClassSlot *slotRemoved);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H
