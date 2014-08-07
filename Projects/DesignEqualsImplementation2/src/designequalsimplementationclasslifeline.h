#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H

#include <QObject>
#include <QPointF>
#include <QList>
#include <QMap>

class DesignEqualsImplementationClass;
class DesignEqualsImplementationClassSlot;
class DesignEqualsImplementationUseCase;
class HasA_Private_Classes_Member;

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

    explicit DesignEqualsImplementationClassLifeLine(DesignEqualsImplementationClass *designEqualsImplementationClass, /*TODOinstancing: DesignEqualsImplementationClassInstance *myInstanceInClassThatHasMe, */DesignEqualsImplementationUseCase *parentUseCase, QPointF position, QObject *parent = 0);
    //explicit DesignEqualsImplementationClassLifeLine(const QString &instanceVariableName, DesignEqualsImplementationClass *designEqualsImplementationClass, /*TODOinstancing: DesignEqualsImplementationClassInstance *myInstanceInClassThatHasMe, */DesignEqualsImplementationUseCase *parentUseCase, QPointF position, QObject *parent = 0);
    //explicit DesignEqualsImplementationClassLifeLine(DesignEqualsImplementationClass *designEqualsImplementationClass, DesignEqualsImplementationClassSlot *firstSlot, HasA_Private_Classes_Members_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline, QPointF position, QObject *parent = 0);
    QPointF position() const;
    DesignEqualsImplementationClass *designEqualsImplementationClass() const;
    DesignEqualsImplementationUseCase *parentUseCase() const;
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
    HasA_Private_Classes_Member *instanceInOtherClassIfApplicable() const;
    void setInstanceInOtherClassIfApplicable(HasA_Private_Classes_Member *instanceInOtherClassIfApplicable);

    QString instanceVariableName();
protected:
    //friend class DesignEqualsImplementationClassLifeLineUnitOfExecution;
    DesignEqualsImplementationClass *m_DesignEqualsImplementationClass;
    //TODOinstancing: DesignEqualsImplementationClassInstance *m_MyInstanceInClassThatHasMe;
    QPointF m_Position;
    MySlotsAppearingInClassLifeLine_List m_MySlotsAppearingInClassLifeLine;
    DesignEqualsImplementationUseCase *m_ParentUseCase;

    DesignEqualsImplementationClassInstanceTypeEnum m_InstanceType;
    HasA_Private_Classes_Member *m_InstanceInOtherClassIfApplicable;
private:
    void privateConstructor(DesignEqualsImplementationClass *designEqualsImplementationClass, DesignEqualsImplementationUseCase *parentUseCase, QPointF position);
signals:
    void slotInsertedIntoClassLifeLine(int insertIndex, DesignEqualsImplementationClassSlot *slot);
    void slotRemovedFromClassLifeLine(DesignEqualsImplementationClassSlot *slotRemoved);
public slots:
    void createNewHasAPrivateMemberAndAssignItAsClassLifelineInstance(DesignEqualsImplementationClass *parentClassChosenToGetNewHasAprivateMember, DesignEqualsImplementationClass *typeOfNewPrivateHasAMember, const QString &nameOfNewPrivateHasAMember);
    void assignPrivateMemberAsClassLifelineInstance(HasA_Private_Classes_Member *chosenExistingHasA_Private_Classes_Member);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINE_H
