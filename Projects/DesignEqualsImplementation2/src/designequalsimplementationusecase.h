#ifndef DESIGNEQUALSIMPLEMENTATIONUSECASE_H
#define DESIGNEQUALSIMPLEMENTATIONUSECASE_H

#include <QObject>
#include <QList>
#include <QPair>
#include <QPointF>

#include "designequalsimplementationclassslot.h"
#include "designequalsimplementationclasssignal.h"
#include "signalemissionorslotinvocationcontextvariables.h"

class DesignEqualsImplementationActor;
class DesignEqualsImplementationClassLifeLine;

class DesignEqualsImplementationUseCase : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationUseCase(QObject *parent = 0);

    enum UseCaseEventTypeEnum
    {
        UseCaseSlotEventType,
        UseCaseSignalEventType,
        UseCaseSignalSlotEventType
    };

    //TODOoptional: private + getter/setter blah
    QString Name;
    QList<QPair<UseCaseEventTypeEnum, QObject*> > OrderedUseCaseEvents;
    DesignEqualsImplementationClassSlot *SlotWithCurrentContext;
    DesignEqualsImplementationClassSignal *ExitSignal;

    void addEvent(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal);
    void addEvent(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionContextVariables = SignalEmissionOrSlotInvocationContextVariables());
    void setExitSignal(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, const SignalEmissionOrSlotInvocationContextVariables &exitSignalEmissionContextVariables = SignalEmissionOrSlotInvocationContextVariables());
    bool generateSourceCode(const QString &destinationDirectoryPath);
    ~DesignEqualsImplementationUseCase();
private:
    DesignEqualsImplementationClassSlot *m_SlotWithExitSignalCurrentlyInItsOrderedListOfStatements;
    int m_ExitSignalsIndexIntoOrderedListOfStatements;

    QList<DesignEqualsImplementationClassLifeLine*> m_ClassLifeLines;

    void addEventPrivate(UseCaseEventTypeEnum useCaseEventType, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot = SignalEmissionOrSlotInvocationContextVariables());
    void addEventPrivateWithoutUpdatingExitSignal(UseCaseEventTypeEnum useCaseEventType, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot = SignalEmissionOrSlotInvocationContextVariables());
signals:
    void actorAdded(DesignEqualsImplementationActor *actor);
    void classLifeLineAdded(DesignEqualsImplementationClassLifeLine *newClassLifeLine);
    void eventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventType, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalOrSlot_contextVariables_AndTargetSlotVariableNameInCurrentContextWhenSlot);
public slots:
    void addActorToUseCase(QPointF position);
    void addClassToUseCase(DesignEqualsImplementationClass *classToAddToUseCase, QPointF position);
    void addSlotInvocationEvent(DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot, const SignalEmissionOrSlotInvocationContextVariables &slotInvocationContextVariables = SignalEmissionOrSlotInvocationContextVariables());
};
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationUseCase &useCase);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationUseCase &useCase);
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationUseCase *&useCase);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationUseCase *&useCase);

Q_DECLARE_METATYPE(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum)

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
