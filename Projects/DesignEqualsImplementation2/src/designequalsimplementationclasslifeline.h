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
    enum DesignEqualsImplementationClassInstanceTypeEnum
    {
        //a) no instance chosen
        NoInstanceChosen

        //b) use case's root class lifeline/instance
        , UseCasesRootClassLifeline

        //c) child of some other class lifeline/instance
        , ChildMemberOfOtherClassLifeline //OLD-mb: denotes m_ParentInstance is valid
    };

    explicit DesignEqualsImplementationClassLifeLine(DesignEqualsImplementationClass *designEqualsImplementationClass, /*TODOinstancing: DesignEqualsImplementationClassInstance *myInstanceInClassThatHasMe, */QPointF position, QObject *parent = 0);
    explicit DesignEqualsImplementationClassLifeLine(const QString &instanceVariableName, DesignEqualsImplementationClass *designEqualsImplementationClass, /*TODOinstancing: DesignEqualsImplementationClassInstance *myInstanceInClassThatHasMe, */QPointF position, QObject *parent = 0);
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
    bool hasBeenAssignedInstance();
    void setInstanceType(DesignEqualsImplementationClassInstanceTypeEnum instanceType);
    QString instanceVariableName() const;
    void setInstanceVariableName(const QString &instanceVariableName);
protected:
    //friend class DesignEqualsImplementationClassLifeLineUnitOfExecution;
    DesignEqualsImplementationClass *m_DesignEqualsImplementationClass;
    //TODOinstancing: DesignEqualsImplementationClassInstance *m_MyInstanceInClassThatHasMe;
    QPointF m_Position;
    MySlotsAppearingInClassLifeLine_List m_MySlotsAppearingInClassLifeLine;

    DesignEqualsImplementationClassInstanceTypeEnum m_InstanceType;
    QString m_InstanceVariableName;
private:
    void privateConstructor(DesignEqualsImplementationClass *designEqualsImplementationClass, QPointF position);
signals:
    void slotInsertedIntoClassLifeLine(int insertIndex, DesignEqualsImplementationClassSlot *slot);
    void slotRemovedFromClassLifeLine(DesignEqualsImplementationClassSlot *slotRemoved);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H
