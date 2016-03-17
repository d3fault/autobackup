#include "designequalsimplementationslotinvocationstatement.h"

#include <QMetaObject>
#include <QDataStream>

#include "designequalsimplementationclassslot.h"
#include "designequalsimplementationclass.h"
#include "designequalsimplementationclasslifeline.h"

//TODOreq: slot invocations should own (or at least know/reference (*****pointers**** would be easiest, since user/ui may change the variable name the future)) the variable name ("m_Bar") of the object whose slot is being invoked
DesignEqualsImplementationSlotInvocationStatement::DesignEqualsImplementationSlotInvocationStatement(DesignEqualsImplementationClassLifeLine *classLifelineWhoseSlotIsAboutToBeInvoked, DesignEqualsImplementationClassSlot *slotToInvoke, const SignalEmissionOrSlotInvocationContextVariables &slotInvocationContextVariables)
    : IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements(SlotInvokeStatementType) //is this necessary or is it called for me?
    , m_ClassLifelineWhoseSlotIsToBeInvoked(classLifelineWhoseSlotIsAboutToBeInvoked)
    , m_SlotToInvoke(slotToInvoke)
    , m_SlotInvocationContextVariables(slotInvocationContextVariables)
{ }
DesignEqualsImplementationSlotInvocationStatement::~DesignEqualsImplementationSlotInvocationStatement()
{ }
QString DesignEqualsImplementationSlotInvocationStatement::toRawCppWithoutEndingSemicolon()
{
    int currentArgumentIndex = 0;
    int maxArgs = m_SlotToInvoke->arguments().size();
    QString argumentsString;
    Q_FOREACH(const QString &currentArgumentName, m_SlotInvocationContextVariables)
    {
        const QByteArray &argTypeCstr = m_SlotToInvoke->arguments().at(currentArgumentIndex)->type->Name.toUtf8();
        argumentsString.append(", Q_ARG(" + QMetaObject::normalizedType(argTypeCstr.constData()) + ", " + currentArgumentName + ")");
        ++currentArgumentIndex;
        if(currentArgumentIndex == maxArgs)
            break; //prevent potential out of bounds 3 lines up: Arguments.at -- TODOoptional: sanitize ui so they can't provide more arguments than the slot will take (TODOoptional: sanitize deserialization for same thing)
    }
    return "QMetaObject::invokeMethod(" + m_ClassLifelineWhoseSlotIsToBeInvoked->instanceVariableName() + ", \"" + m_SlotToInvoke->Name + "\"" + argumentsString + ")"; //TODOmb: might need to normalize the "type" passed as first var to Q_ARG. ex: "const QString &" becomes "QString". I'm unsure, and anyways pretty sure Qt has a method somewhere for that in it's QMeta* framework (there's probably tons of QMeta* stuff I could be [re-]using, but... but... but... well I guess I don't want to have to depend on it (using one method here and there such as type normalization as just mentioned is fine) because.... idfk... diamond inheritence problems? Since I'm not depending on QMeta* stuff, I can hack away [without having to recompile all of fucking Qt]))
}
const SignalEmissionOrSlotInvocationContextVariables &DesignEqualsImplementationSlotInvocationStatement::slotInvocationContextVariables()
{
    return m_SlotInvocationContextVariables;
}
DesignEqualsImplementationClassSlot *DesignEqualsImplementationSlotInvocationStatement::slotToInvoke()
{
    return m_SlotToInvoke;
}
void DesignEqualsImplementationSlotInvocationStatement::streamIn(DesignEqualsImplementationProject *project, QDataStream &in)
{
    m_ClassLifelineWhoseSlotIsToBeInvoked = DesignEqualsImplementationClassLifeLine::streamInClassLifelineReference(project, in);
    m_SlotToInvoke = DesignEqualsImplementationClassSlot::streamInSlotReference(project, in);
    in >> m_SlotInvocationContextVariables; //TODOoptimization: dictionary/id (just like classes/slots/etc) for the variable names. fuck it for now
}
void DesignEqualsImplementationSlotInvocationStatement::streamOut(DesignEqualsImplementationProject *project, QDataStream &out)
{
    DesignEqualsImplementationClassLifeLine::streamOutClassLifelineReference(project, m_ClassLifelineWhoseSlotIsToBeInvoked, out);
    DesignEqualsImplementationClassSlot::streamOutSlotReference(project, m_SlotToInvoke, out);
    out << m_SlotInvocationContextVariables;
}
DesignEqualsImplementationClassLifeLine *DesignEqualsImplementationSlotInvocationStatement::classLifelineWhoseSlotIsToBeInvoked() const
{
    return m_ClassLifelineWhoseSlotIsToBeInvoked;
}
