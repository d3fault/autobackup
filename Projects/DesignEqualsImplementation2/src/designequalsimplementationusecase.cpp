#include "designequalsimplementationusecase.h"

#include <QDataStream>

typedef QPair<DesignEqualsImplementationUseCase::UseCaseEventTypeEnum, QObject*> EventAndTypeTypedef;

#define DesignEqualsImplementationUseCase_QDS(direction, qds, useCase) \
qds direction numOrderedUseCaseEvents; \
qds direction hasExitSignal;

//TODOreq: by far the most difficult class to design/[de-]serialize/generate-from. the rest are just busywork by comparison
//TODOoptimization: class diagram serialized first (although use cases must be factored into that!), serves as a simple library or whatever so that use cases only later have to refer to a signal by id/etc rather than redefine/reserialize the signal/slot/etc every time it is referenced
DesignEqualsImplementationUseCase::DesignEqualsImplementationUseCase(QObject *parent)
    : QObject(parent)
    , ExitSignal(0)
{ }
//Overload: Use Case entry point and also normal slot invocation from within another slot
void DesignEqualsImplementationUseCase::addEvent(DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot)
{
    addEventPrivate(UseCaseSlotEventType, designEqualsImplementationClassSlot);
}
//Overload: Signals with no listeners in this use case
void DesignEqualsImplementationUseCase::addEvent(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal)
{
    addEventPrivate(UseCaseSignalEventType, designEqualsImplementationClassSignal);
}
//Overload: Signal emitted during normal slot execution, slot handler for that signal is relevant to use case
void DesignEqualsImplementationUseCase::addEvent(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot)
{
    addEventPrivate(UseCaseSignalSlotEventType, new SignalSlotCombinedEventHolder(designEqualsImplementationClassSignal, designEqualsImplementationClassSlot));
}
void DesignEqualsImplementationUseCase::setExitSignal(DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal)
{
    ExitSignal = designEqualsImplementationClassSignal;
}
DesignEqualsImplementationUseCase::~DesignEqualsImplementationUseCase()
{
    //TODOreq: probably don't need to delete here AND in class diagram perspective, but probably doesn't hurt also
    Q_FOREACH(EventAndTypeTypedef eventAndType, OrderedUseCaseEvents)
    {
        delete eventAndType.second;
    }
    if(ExitSignal)
        delete ExitSignal;
}
void DesignEqualsImplementationUseCase::addEventPrivate(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventType, QObject *event)
{
    OrderedUseCaseEvents.append(qMakePair(useCaseEventType, event));
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationUseCase &useCase)
{
    qint32 numOrderedUseCaseEvents = useCase.OrderedUseCaseEvents.size();
    bool hasExitSignal = useCase.ExitSignal != 0;
    DesignEqualsImplementationUseCase_QDS(<<, out, useCase)
    if(hasExitSignal)
        out << *useCase.ExitSignal;
    for(qint32 i = 0; i < numOrderedUseCaseEvents; ++i)
    {
        EventAndTypeTypedef eventAndType = useCase.OrderedUseCaseEvents.at(i);
        DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventType = eventAndType.first;
        out << static_cast<quint8>(useCaseEventType);
        switch(useCaseEventType)
        {
        case DesignEqualsImplementationUseCase::UseCaseSlotEventType:
            out << *(static_cast<DesignEqualsImplementationClassSlot*>(eventAndType.second));
            break;
        case DesignEqualsImplementationUseCase::UseCaseExitSignalEventType: //already handled
        case DesignEqualsImplementationUseCase::UseCaseSignalEventType:
            out << *(static_cast<DesignEqualsImplementationClassSignal*>(eventAndType.second));
            break;
        case DesignEqualsImplementationUseCase::UseCaseSignalSlotEventType:
            out << *(static_cast<SignalSlotCombinedEventHolder*>(eventAndType.second));
            break;
        }
    }
    return out;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationUseCase &useCase)
{
    qint32 numOrderedUseCaseEvents;
    bool hasExitSignal;
    DesignEqualsImplementationUseCase_QDS(>>, in, useCase)
    if(hasExitSignal)
    {
        DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal = new DesignEqualsImplementationClassSignal(&useCase);
        in >> *designEqualsImplementationClassSignal;
        useCase.ExitSignal = designEqualsImplementationClassSignal;
    }
    for(qint32 i = 0; i < numOrderedUseCaseEvents; ++i)
    {
        quint8 useCaseEventTypeAsQuint8;
        in >> useCaseEventTypeAsQuint8;
        DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventType = static_cast<DesignEqualsImplementationUseCase::UseCaseEventTypeEnum>(useCaseEventTypeAsQuint8);
        switch(useCaseEventType)
        {
        case DesignEqualsImplementationUseCase::UseCaseSlotEventType:
            {
                DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot = new DesignEqualsImplementationClassSlot(&useCase); //TODOreq: weird parenting here and below instantiations also. not class for parent as usual
                in >> *designEqualsImplementationClassSlot;
                useCase.OrderedUseCaseEvents.append(qMakePair(useCaseEventType, designEqualsImplementationClassSlot));
            }
            break;
        case DesignEqualsImplementationUseCase::UseCaseExitSignalEventType: //already handled
        case DesignEqualsImplementationUseCase::UseCaseSignalEventType:
            {
                DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal = new DesignEqualsImplementationClassSignal(&useCase);
                in >> *designEqualsImplementationClassSignal;
                useCase.OrderedUseCaseEvents.append(qMakePair(useCaseEventType, designEqualsImplementationClassSignal));
            }
            break;
        case DesignEqualsImplementationUseCase::UseCaseSignalSlotEventType:
            {
                SignalSlotCombinedEventHolder *signalSlotCombinedEventHolder = new SignalSlotCombinedEventHolder(&useCase);
                in >> *signalSlotCombinedEventHolder;
                useCase.OrderedUseCaseEvents.append(qMakePair(useCaseEventType, signalSlotCombinedEventHolder));
            }
            break;
        }
    }
    return in;
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationUseCase *&useCase)
{
    return out << *useCase;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationUseCase *&useCase)
{
    useCase = new DesignEqualsImplementationUseCase(); //TODOreq: no parent at this context
    return in >> *useCase;
}
QDataStream &operator<<(QDataStream &out, const SignalSlotCombinedEventHolder &useCase)
{
    out << *useCase.m_DesignEqualsImplementationClassSignal;
    out << *useCase.m_DesignEqualsImplementationClassSlot;
    return out;
}
QDataStream &operator>>(QDataStream &in, SignalSlotCombinedEventHolder &useCase)
{
    in >> *useCase.m_DesignEqualsImplementationClassSignal;
    in >> *useCase.m_DesignEqualsImplementationClassSlot;
    return in;
}
