#ifndef DESIGNEQUALSIMPLEMENTATIONUSECASE_H
#define DESIGNEQUALSIMPLEMENTATIONUSECASE_H

#include <QObject>
#include <QList>
#include <QPair>

#include "designequalsimplementationclassslot.h"
#include "designequalsimplementationclasssignal.h"

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
    QList<QPair<UseCaseEventTypeEnum, QObject*> > OrderedUseCaseEvents;
    DesignEqualsImplementationClassSignal *ExitSignal;

    void addEvent(DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot);
    void addEvent(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal);
    void addEvent(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot);
    void setExitSignal(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal);
    ~DesignEqualsImplementationUseCase();

    //TODOoptional: private + getter/setter blah
private:
    void addEventPrivate(UseCaseEventTypeEnum useCaseEventType, QObject *event);
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
    ~SignalSlotCombinedEventHolder() { delete m_DesignEqualsImplementationClassSignal; delete m_DesignEqualsImplementationClassSlot; }
    DesignEqualsImplementationClassSignal *m_DesignEqualsImplementationClassSignal;
    DesignEqualsImplementationClassSlot *m_DesignEqualsImplementationClassSlot;
};
QDataStream &operator<<(QDataStream &out, const SignalSlotCombinedEventHolder &useCase);
QDataStream &operator>>(QDataStream &in, SignalSlotCombinedEventHolder &useCase);

#endif // DESIGNEQUALSIMPLEMENTATIONUSECASE_H
