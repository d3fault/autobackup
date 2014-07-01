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

    //TODOoptional: private + getter/setter blah
    QString Name;
    QList<QPair<UseCaseEventTypeEnum, QObject*> > OrderedUseCaseEvents;
    DesignEqualsImplementationClassSlot *SlotWithCurrentContext;
    DesignEqualsImplementationClassSignal *ExitSignal;

    void addEvent(DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot, const QList<QString> &orderedListOfNamesOfVariablesWithinScopeWhenSlotInvocationOccurred_ToUseForSlotInvocationArguments = QList<QString>());
    void addEvent(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal);
    void addEvent(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot);
    void setExitSignal(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal);
    bool generateSourceCode(const QString &destinationDirectoryPath);
    ~DesignEqualsImplementationUseCase();
private:
    void addEventPrivate(UseCaseEventTypeEnum useCaseEventType, QObject *event, const QList<QString> &SLOT_ONLY_orderedListOfNamesOfVariablesWithinScopeWhenSlotInvocationOccurred_ToUseForSlotInvocationArguments = QList<QString>());
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
