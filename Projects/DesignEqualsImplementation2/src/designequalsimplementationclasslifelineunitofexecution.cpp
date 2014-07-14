#include "designequalsimplementationclasslifelineunitofexecution.h"

//Units of execution are the building blocks of use cases. Every use case has at least one unit of execution. I am now working on a UML Design mode where signals/slots are used exclusively, and every time a slot is invoked via invokeMethod or signal/slot, a new unit of execution is started (not necessarily, but we must assume so in order to be thread safe). In the future the UML designer could be "simple objects based" where "invoke a method" does not begin a unit of execution. That UML design mode would be completely independent of Qt, it would generate plain C/C++ objects, method calls, properties, etc. The two modes must be intertwined, noticed, and differentiable. For now though this higher level Qt-signals-slots code generator is still pretty valuable. The plain C++ object stuff is usually procedural by comparison (each slot makes is pretty much a single procedural "process"), so it wouldn't hurt too much to have to do it all in drop-down C++ mode (since it would probably require that anyways)
DesignEqualsImplementationClassLifeLineUnitOfExecution::DesignEqualsImplementationClassLifeLineUnitOfExecution(DesignEqualsImplementationClassLifeLine *designEqualsImplementationClassLifeLine, QObject *parent)
    : QObject(parent)
    , m_MethodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution(0)
    , m_DesignEqualsImplementationClassLifeLine(designEqualsImplementationClassLifeLine)
{ }
DesignEqualsImplementationClassLifeLine *DesignEqualsImplementationClassLifeLineUnitOfExecution::designEqualsImplementationClassLifeLine() const
{
    return m_DesignEqualsImplementationClassLifeLine;
}
void DesignEqualsImplementationClassLifeLineUnitOfExecution::setMethodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution(IDesignEqualsImplementationHaveOrderedListOfStatements *methodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution)
{
    m_MethodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution = methodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution;

    //TODOreq: before the connect: disconnect all old listeners to the first signal? disconnect all listeners to second signal? (daisy chaining)
    connect(methodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution->asQObject(), SIGNAL(statementInserted(int,IDesignEqualsImplementationStatement*)), this, SIGNAL(statementInserted(int,IDesignEqualsImplementationStatement*)));
}
IDesignEqualsImplementationHaveOrderedListOfStatements *DesignEqualsImplementationClassLifeLineUnitOfExecution::methodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution() const
{
    return m_MethodWithOrderedListOfStatements_Aka_EntryPointToUnitOfExecution;
}
