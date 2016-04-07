#include "signalemissionorslotinvocationcontextvariables.h"

#include <QDataStream>

#include "designequalsimplementationclassmethodargument.h"

//TODOoptional: neither of these methods need to be static
void SignalEmissionOrSlotInvocationContextVariables::streamIn(DesignEqualsImplementationProject *project, SignalEmissionOrSlotInvocationContextVariables *contextVariables, QDataStream &in)
{
    int size;
    in >> size;
    for(int i = 0; i < size; ++i)
        contextVariables->append(TypeInstance::streamInReferenceAndReturnPointerToIt(in, project));
#if 0
    quint8 typeInstanceCategory;
    in >> typeInstanceCategory;

    if(typeInstanceCategory == 0)
    {
        //DesignEqualsImplementationClassMethodArgument
        quint8 functionTypeId;
        in >> functionTypeId;
        FunctionType functionType = static_cast<FunctionType>(functionTypeId);

    }
    else if(typeInstanceCategory == 1)
    {
        //NonFunctionMember
    }
    else
        qFatal("Couldn't find out type instnace category");
#endif
}
void SignalEmissionOrSlotInvocationContextVariables::streamOut(DesignEqualsImplementationProject *project, SignalEmissionOrSlotInvocationContextVariables *contextVariables, QDataStream &out)
{
    out << contextVariables->size();
    Q_FOREACH(TypeInstance *currentContextVariable, *contextVariables)
    {
        //out << currentContextVariable->p

        currentContextVariable->streamOutReference(out, project);

#if 0
        if(const DesignEqualsImplementationClassMethodArgument *typeInstanceAsFunctionArgument = qobject_cast<const DesignEqualsImplementationClassMethodArgument*>(currentContextVariable))
        {
            out << static_cast<quint8>(0);
            out << static_cast<quint8>(typeInstanceAsFunctionArgument->ParentFunction->functionType());
            out << typeInstanceAsFunctionArgument->ParentFunction->functionSerializationId();
            out << typeInstanceAsFunctionArgument->ParentFunction->argumentSerializationIdForArgument(typeInstanceAsFunctionArgument); //TODOreq: give a good hard think about whether or not using the same context variable twice (or more) in one function call will need to be [de]serialized specially
        }
        else if(const NonFunctionMember *typeInstnaceAsNonFunctionMember = qobject_cast<const NonFunctionMember*>(currentContextVariable))
        {

        }
        else
            qFatal("Couldn't cast TypeInstance into an inherited type");
#endif
    }
}
