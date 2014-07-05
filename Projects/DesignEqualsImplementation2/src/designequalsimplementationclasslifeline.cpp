#include "designequalsimplementationclasslifeline.h"

//Tempted to say this should inherit from 'class', but idk fo sho (plus that would require refactor (frontend specifies TYPE, backend factories it (whereas now i'm just passing one pointer around heh)) so fuck it for now)
//TODOreq: need to define the concept of a "Unit of Execution". Multiple class lifelines can share a unit of execution (they do, if they are on the same thread and use AutoConnection). A unit of execution ends when an event on a different thread is added and no more events on that OUR thread are added after that (except as a result of the OTHER thread jumping back to us (same life line, now different unit of execution)). An object(or group of objects sharing a unit of execution) should never be in an invalid state when finishing a unit of execution, following this rule is the easiest/sanest way to achieve thread safety. One one hand, my initial Foo/Bar example does not require threading (and therefore the concept of unit of execution), but on the other I may find myself refactoring for it later since I definitely do want it. And of course: KISS xD
DesignEqualsImplementationClassLifeLine::DesignEqualsImplementationClassLifeLine(DesignEqualsImplementationClass *designEqualsImplementationClass, QPointF position, QObject *parent)
    : QObject(parent)
    , m_DesignEqualsImplementationClass(designEqualsImplementationClass)
    , m_Position(position) //Could just keep one qreal "horizontalPosition"
{ }
QPointF DesignEqualsImplementationClassLifeLine::position() const
{
    return m_Position;
}
DesignEqualsImplementationClass *DesignEqualsImplementationClassLifeLine::designEqualsImplementationClass() const
{
    return m_DesignEqualsImplementationClass;
}
