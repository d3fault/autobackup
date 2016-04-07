#include "idesignequalsimplementationmethod.h"

DesignEqualsImplementationClassMethodArgument *IDesignEqualsImplementationMethod::createNewArgument(DesignEqualsImplementationType *argumentType, const QString &qualifiedArgumentTypeString, const QString &argumentVariableName)
{
    DesignEqualsImplementationClassMethodArgument *newArgument = new DesignEqualsImplementationClassMethodArgument(this, argumentType, qualifiedArgumentTypeString, argumentVariableName, asQObject());
    m_Arguments.append(newArgument);
    return newArgument;
}
QList<DesignEqualsImplementationClassMethodArgument *> IDesignEqualsImplementationMethod::arguments() const
{
    return m_Arguments;
}
QList<ParsedTypeInstance> IDesignEqualsImplementationMethod::argumentsAsFunctionArgumentTypedefList() const
{
    QList<ParsedTypeInstance> ret;
    Q_FOREACH(DesignEqualsImplementationClassMethodArgument* currentArgument, m_Arguments)
    {
        ParsedTypeInstance entry;
        entry.NonQualifiedType = currentArgument->type->Name;
        entry.QualifiedType = currentArgument->qualifiedType();
        entry.Name = currentArgument->VariableName;
        ret.append(entry);
    }
    return ret;
}
QString IDesignEqualsImplementationMethod::methodSignatureWithoutReturnType(MethodSignatureFlagsEnum methodSignatureFlagsEnum)
{
    return Name + "(" + argumentsToCommaSeparatedString(methodSignatureFlagsEnum) + ")";
}
//TODOmb: default variables for slots (only in header obviously), BUT not compatible with Qt5's new connect syntax anyways so.... (so don't use default variables i guess)
QString IDesignEqualsImplementationMethod::argumentsToCommaSeparatedString(MethodSignatureFlagsEnum methodSignatureFlagsEnum)
{
    QString argString;
    bool firstArg = true;
    Q_FOREACH(DesignEqualsImplementationClassMethodArgument *currentArgument, arguments())
    {
        if(!firstArg)
            argString.append(", ");

        if(methodSignatureFlagsEnum == MethodSignatureForVisualAppearanceContainsArgumentVariableNames)
        {
            argString.append(currentArgument->preferredTextualRepresentationOfTypeAndVariableTogether());
        }
        else if(methodSignatureFlagsEnum == MethodSignatureNormalizedAndDoesNotContainArgumentsVariableNames)
        {
            const QByteArray &argTypeCstr = currentArgument->type->Name.toUtf8();
            argString.append(QMetaObject::normalizedType(argTypeCstr.constData()));
        }


        firstArg = false;
    }
    return argString;
}
int IDesignEqualsImplementationMethod::serializationArgumentIdForArgument(DesignEqualsImplementationClassMethodArgument *argument)
{
    return m_Arguments.indexOf(argument);
}
DesignEqualsImplementationClassMethodArgument *IDesignEqualsImplementationMethod::argumentInstantiationFromSerializedArgumentId(int argumentId)
{
    return m_Arguments.at(argumentId);
}
