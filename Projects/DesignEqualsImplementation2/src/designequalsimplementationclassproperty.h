#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSPROPERTY_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSPROPERTY_H

#include "type.h"
#include "ihavetypeandvariablenameandpreferredtextualrepresentation.h"

class DesignEqualsImplementationClassProperty : public TypeInstance, public IHaveTypeAndVariableNameAndPreferredTextualRepresentation
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassProperty(QObject *parent = 0);
    explicit DesignEqualsImplementationClassProperty(const QString &propertyType, const QString &propertyName, bool hasInit, const QString &optionalInit, bool readOnly, bool notifiesOnChange, QObject *parent = 0);
    virtual QString typeString() { return Type; }

    //TODOoptional: private + getter/setter blah
    QString Type;
    QString Name;
    bool HasInit;
    QString OptionalInit;
    bool ReadOnly;
    bool NotifiesOnChange;
    //TODOoptional: a few others, such as "reset" (which should be combined with "default"/initial-value imo)
};
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassProperty &designEqualsImplementationClassProperty);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassProperty &designEqualsImplementationClassProperty);
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassProperty *designEqualsImplementationClassProperty);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassProperty *designEqualsImplementationClassProperty);

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSPROPERTY_H
