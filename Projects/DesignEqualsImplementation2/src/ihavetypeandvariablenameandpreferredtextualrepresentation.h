#ifndef IHAVETYPEANDVARIABLENAMEANDPREFERREDTEXTUALREPRESENTATION_H
#define IHAVETYPEANDVARIABLENAMEANDPREFERREDTEXTUALREPRESENTATION_H

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
    QString preferredTextualRepresentation()
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

#endif // IHAVETYPEANDVARIABLENAMEANDPREFERREDTEXTUALREPRESENTATION_H
