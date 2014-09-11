#include "designequalsimplementationactor.h"

//An actor is a pseudo unit of execution. Only contains "entry point (signal-esq, but code for this signal is never generated (the slot that it invokes is generated however))" and "exit point" (the reverse: signal generated, slot/unit-of-execution not). I wonder if making them inherit the same type would ease coding... or just lead to more confusion xD
DesignEqualsImplementationActor::DesignEqualsImplementationActor(QPointF position, QObject *parent) //pretty fucking worthless class...
    : QObject(parent)
    , m_Position(position)
{ }
QPointF DesignEqualsImplementationActor::position() const
{
    return m_Position;
}
