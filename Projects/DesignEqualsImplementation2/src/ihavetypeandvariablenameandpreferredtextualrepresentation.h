#ifndef IHAVETYPEANDVARIABLENAMEANDPREFERREDTEXTUALREPRESENTATION_H
#define IHAVETYPEANDVARIABLENAMEANDPREFERREDTEXTUALREPRESENTATION_H

#if 1
#include <QString>
#include <QMetaType>

class IHaveTypeAndVariableNameAndPreferredTextualRepresentation
{
public:
    explicit IHaveTypeAndVariableNameAndPreferredTextualRepresentation() { }
    explicit IHaveTypeAndVariableNameAndPreferredTextualRepresentation(const QString &variableName) : VariableName(variableName) { }
    virtual ~IHaveTypeAndVariableNameAndPreferredTextualRepresentation() { }

    //TODOoptional: private + getter/setter blah
    QString VariableName;
    virtual QString typeString()=0;
    QString preferredTextualRepresentationOfTypeAndVariableTogether()
    {
        QString ret(typeString());
        if(ret.endsWith("&") || ret.endsWith("*")) //etc
        {
            return (ret + VariableName);
        }
        else
        {
            return (ret + " " + VariableName);
        }
    }
};

Q_DECLARE_METATYPE(IHaveTypeAndVariableNameAndPreferredTextualRepresentation*)
#endif

#endif // IHAVETYPEANDVARIABLENAMEANDPREFERREDTEXTUALREPRESENTATION_H
