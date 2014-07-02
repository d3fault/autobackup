#ifndef DESIGNEQUALSIMPLEMENTATIONUSECASE_H
#define DESIGNEQUALSIMPLEMENTATIONUSECASE_H

#include <QObject>
#include <QList>
#include <QPair>

#include "designequalsimplementationclassslot.h"
#include "designequalsimplementationclasssignal.h"
#include "slotinvocationcontextvariables.h"

class DesignEqualsImplementationUseCase : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationUseCase(QObject *parent = 0);

    enum UseCaseEventTypeEnum
    {
        UseCaseSlotEventType,
        UseCaseSignalEventType,
        UseCaseSignalSlotEventType,
        UseCaseExitSignalEventType
    };

    //TODOoptional: private + getter/setter blah
    QString Name;
    QList<QPair<UseCaseEventTypeEnum, QObject*> > OrderedUseCaseEvents;
    DesignEqualsImplementationClassSlot *SlotWithCurrentContext;
    DesignEqualsImplementationClassSignal *ExitSignal;

    void addEvent(DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot, const SlotInvocationContextVariables &slotInvocationContextVariables = SlotInvocationContextVariables());
    void addEvent(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal);
    void addEvent(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot);
    void setExitSignal(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal);
    bool generateSourceCode(const QString &destinationDirectoryPath);
    ~DesignEqualsImplementationUseCase();
private:
    void addEventPrivate(UseCaseEventTypeEnum useCaseEventType, QObject *event, const SlotInvocationContextVariables &SLOT_ONLY_slotInvocationContextVariables = SlotInvocationContextVariables());
};
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationUseCase &useCase);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationUseCase &useCase);
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationUseCase *&useCase);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationUseCase *&useCase);

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
