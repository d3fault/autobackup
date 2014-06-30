#ifndef DESIGNEQUALSIMPLEMENTATIONUSECASEEVENT_H
#define DESIGNEQUALSIMPLEMENTATIONUSECASEEVENT_H

#include <QObject>

#include "designequalsimplementationclasssignal.h"
#include "designequalsimplementationclassslot.h"

class DesignEqualsImplementationUseCaseEvent : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationUseCaseEvent(QObject *parent = 0);

    //TODOoptional: private + getter/setter blah

    //MAYBE WRONG BUT TYPING ANYWAYS (wrong because Actor does not have signal/slot concept (but could...?)):
    DesignEqualsImplementationClassSignal Sender;
    DesignEqualsImplementationClassSlot Receiver;
};

#endif // DESIGNEQUALSIMPLEMENTATIONUSECASEEVENT_H
