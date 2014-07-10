#ifndef DESIGNEQUALSIMPLEMENTATIONUSECASE_H
#define DESIGNEQUALSIMPLEMENTATIONUSECASE_H

#include <QObject>
#include <QList>
#include <QPair>
#include <QPointF>

#include "designequalsimplementationclassslot.h"
#include "designequalsimplementationclasssignal.h"
#include "signalemissionorslotinvocationcontextvariables.h"

class DesignEqualsImplementationProject;
class HasA_Private_Classes_Members_ListEntryType;
class DesignEqualsImplementationActor;
class DesignEqualsImplementationClassLifeLine;

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
    QList<QPair<DesignEqualsImplementationUseCase::UseCaseEventTypeEnum, QObject*> > OrderedUseCaseEvents;
    DesignEqualsImplementationClassSlot *SlotWithCurrentContext;
    DesignEqualsImplementationClassSignal *ExitSignal;

    bool generateSourceCode(const QString &destinationDirectoryPath);
    ~DesignEqualsImplementationUseCase();
private:
    DesignEqualsImplementationProject *m_DesignEqualsImplementationProject;
    DesignEqualsImplementationClassSlot *m_SlotWithExitSignalCurrentlyInItsOrderedListOfStatements;
    int m_ExitSignalsIndexIntoOrderedListOfStatements;

    QList<DesignEqualsImplementationClassLifeLine*> m_ClassLifeLines;

    void privateConstructor(DesignEqualsImplementationProject *project);

    void addEventPrivate(UseCaseEventTypeEnum useCaseEventType, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot);
    void addEventPrivateWithoutUpdatingExitSignal(UseCaseEventTypeEnum useCaseEventType, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot = SignalEmissionOrSlotInvocationContextVariables());
signals:
    void actorAdded(DesignEqualsImplementationActor *actor);
    void classLifeLineAdded(DesignEqualsImplementationClassLifeLine *newClassLifeLine);
    void eventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventType, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot);
public slots:
    void addActorToUseCase(QPointF position);
    void addClassToUseCase(DesignEqualsImplementationClass *classToAddToUseCase, HasA_Private_Classes_Members_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject, QPointF position);
    void addSlotInvocationEvent(DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot, const SignalEmissionOrSlotInvocationContextVariables &slotInvocationContextVariables);
    void addSignalSlotActivationEvent(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionContextVariables);
    void addSignalEmitEvent(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionContextVariables);
    void setExitSignal(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, const SignalEmissionOrSlotInvocationContextVariables &exitSignalEmissionContextVariables);
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
