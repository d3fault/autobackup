#ifndef DESIGNEQUALSIMPLEMENTATIONUSECASE_H
#define DESIGNEQUALSIMPLEMENTATIONUSECASE_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QPair>
#include <QPointF>

#include "designequalsimplementationclassslot.h"
#include "designequalsimplementationclasssignal.h"
#include "signalemissionorslotinvocationcontextvariables.h"

class DesignEqualsImplementationProject;
class SignalSlotCombinedEventHolder;
class HasA_Private_Classes_Members_ListEntryType;
class DesignEqualsImplementationActor;
class DesignEqualsImplementationClassLifeLine;
class DesignEqualsImplementationClassLifeLineUnitOfExecution;

class DesignEqualsImplementationUseCase : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationUseCase(QObject *parent = 0);
    explicit DesignEqualsImplementationUseCase(DesignEqualsImplementationProject *project, QObject *parent = 0);

    enum UseCaseEventTypeEnum //TODOoptional: move to guicommon, since also used as dialog mode
    {
        UseCaseSlotEventType,
        UseCaseSignalEventType,
        UseCaseSignalSlotEventType
    };

    DesignEqualsImplementationProject *designEqualsImplementationProject() const;

    //TODOoptional: private + getter/setter blah
    QString Name;
    QList<QPair<DesignEqualsImplementationUseCase::UseCaseEventTypeEnum, QObject*> > SemiOrderedUseCaseEvents;
    //DesignEqualsImplementationClassSlot *SlotWithCurrentContext;
    //DesignEqualsImplementationClassSignal *ExitSignal;

    bool generateSourceCode(const QString &destinationDirectoryPath);
    ~DesignEqualsImplementationUseCase();
private:
    DesignEqualsImplementationProject *m_DesignEqualsImplementationProject;
    DesignEqualsImplementationClassSlot *m_SlotWithExitSignalCurrentlyInItsOrderedListOfStatements;
    int m_ExitSignalsIndexIntoOrderedListOfStatements;

    QList<DesignEqualsImplementationClassLifeLine*> m_ClassLifeLines;
    QList<DesignEqualsImplementationClassLifeLineUnitOfExecution*> m_UnitsOfExecutionMakingApperanceInUseCase; //design ordered top down apperances. will probably use two PointFs in the future for more accurate serializing

    void privateConstructor(DesignEqualsImplementationProject *project);

    void insertEventPrivate(UseCaseEventTypeEnum useCaseEventType, int indexToInsertEventAt, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassLifeLineUnitOfExecution *destinationUnitOfExecution_OrZeroIfDestIsActor, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot);
    //void addEventPrivateWithoutUpdatingExitSignal(UseCaseEventTypeEnum useCaseEventType, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot = SignalEmissionOrSlotInvocationContextVariables());
    DesignEqualsImplementationClassLifeLineUnitOfExecution *insertNewNamedUnitOfExecutionIntoUseCase(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecutionToAddToThisUseCase, DesignEqualsImplementationClassSlot *slotEntryPointThatKindaSortaMakesItNamed);
signals:
    void actorAdded(DesignEqualsImplementationActor *actor);
    void classLifeLineAdded(DesignEqualsImplementationClassLifeLine *newClassLifeLine);

    void slotInvokeEventAdded(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecutionJustMadeForSlot, DesignEqualsImplementationClassSlot *slotUseCaseEvent);
    void signalEmitEventAdded(DesignEqualsImplementationClassSignal *signalUseCaseEvent);
    void signalSlotEventAdded(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecutionJustMadeForSlot, SignalSlotCombinedEventHolder *signalSlotCombinedUseCaseEvent);

    void eventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventType, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot);
public slots:
    void addActorToUseCase(QPointF position);
    void addClassToUseCase(DesignEqualsImplementationClass *classToAddToUseCase, HasA_Private_Classes_Members_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject, QPointF position);
    void insertSlotInvocationEvent(int indexToInsertEventAt, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassLifeLineUnitOfExecution *destinationUnitOfExecution_OrZeroIfDestIsActor, DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot, const SignalEmissionOrSlotInvocationContextVariables &slotInvocationContextVariables);
    void insertSignalSlotActivationEvent(int indexToInsertEventAt, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassLifeLineUnitOfExecution *destinationUnitOfExecution_OrZeroIfDestIsActor, DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionContextVariables);
    void insertSignalEmitEvent(int indexToInsertEventAt, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassLifeLineUnitOfExecution *destinationUnitOfExecution_OrZeroIfDestIsActor, DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionContextVariables);
    void setExitSignal(DesignEqualsImplementationClassLifeLineUnitOfExecution *sourceUnitOfExecution, DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, const SignalEmissionOrSlotInvocationContextVariables &exitSignalEmissionContextVariables);
};
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationUseCase &useCase);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationUseCase &useCase);
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationUseCase *&useCase);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationUseCase *&useCase);

typedef QPair<DesignEqualsImplementationUseCase::UseCaseEventTypeEnum, QObject*> UseCaseEventListEntryType; //OrderedUseCaseEvents has a copy/paste of this type, because I couldn't forward declare it or whatever (wtf)


class SignalSlotCombinedEventHolder : public QObject
{
    Q_OBJECT
public:
    explicit SignalSlotCombinedEventHolder(QObject *parent = 0) : QObject(parent) { }
    explicit SignalSlotCombinedEventHolder(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot, QObject *parent = 0)
        : QObject(parent)
        , m_DesignEqualsImplementationClassSignal(designEqualsImplementationClassSignal)
        , m_DesignEqualsImplementationClassSlot(designEqualsImplementationClassSlot)
    { }
    ~SignalSlotCombinedEventHolder() { /*these two combined children are deleted by class diagram perspective: delete m_DesignEqualsImplementationClassSignal; delete m_DesignEqualsImplementationClassSlot;*/ }
    DesignEqualsImplementationClassSignal *m_DesignEqualsImplementationClassSignal;
    DesignEqualsImplementationClassSlot *m_DesignEqualsImplementationClassSlot;
};
QDataStream &operator<<(QDataStream &out, const SignalSlotCombinedEventHolder &useCase);
QDataStream &operator>>(QDataStream &in, SignalSlotCombinedEventHolder &useCase);

#endif // DESIGNEQUALSIMPLEMENTATIONUSECASE_H
