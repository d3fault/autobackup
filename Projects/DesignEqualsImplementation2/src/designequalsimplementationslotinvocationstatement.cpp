#include "designequalsimplementationslotinvocationstatement.h"

#include "designequalsimplementationclassslot.h"
#include "designequalsimplementationclass.h"

DesignEqualsImplementationSlotInvocationStatement::DesignEqualsImplementationSlotInvocationStatement(DesignEqualsImplementationClassSlot *slotToInvoke, const QList<QString> &orderedListOfNamesOfVariablesWithinScopeWhenSlotInvocationOccurred_ToUseForSlotInvocationArguments)
    : IDesignEqualsImplementationStatement() //is this necessary or is it called for me?
    , m_SlotToInvoke(slotToInvoke)
    , m_OrderedListOfNamesOfVariablesWithinScopeWhenSlotInvocationOccurred_ToUseForSlotInvocationArguments(orderedListOfNamesOfVariablesWithinScopeWhenSlotInvocationOccurred_ToUseForSlotInvocationArguments)
{ }
DesignEqualsImplementationSlotInvocationStatement::~DesignEqualsImplementationSlotInvocationStatement()
{ }
QString DesignEqualsImplementationSlotInvocationStatement::toRawCppWithoutEndingSemicolon()
{
    int currentArgumentIndex = 0;
    int maxArgs = m_SlotToInvoke->Arguments.size();
    QString argumentsString;
    Q_FOREACH(const QString &currentArgumentName, m_OrderedListOfNamesOfVariablesWithinScopeWhenSlotInvocationOccurred_ToUseForSlotInvocationArguments)
    {
        argumentsString.append(", Q_ARG(" + m_SlotToInvoke->Arguments.at(currentArgumentIndex)->Type + ", " + currentArgumentName + ")");
        ++currentArgumentIndex;
        if(currentArgumentIndex == maxArgs)
            break; //prevent potential out of bounds 3 lines up: Arguments.at -- TODOoptional: sanitize ui so they can't provide more arguments than the slot will take (TODOoptional: sanitize deserialization for same thing)
    }
    return "QMetaObject::invokeMethod(" + m_SlotToInvoke->ParentClass->Name + ", " + m_SlotToInvoke->Name + argumentsString + ")";
}
