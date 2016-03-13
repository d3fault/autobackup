#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSPROPERTY_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSPROPERTY_H

#include "type.h"
#include "ihavetypeandvariablenameandpreferredtextualrepresentation.h"

class DesignEqualsImplementationClassProperty : public NonFunctionMember
{
    Q_OBJECT
public:
    //explicit DesignEqualsImplementationClassProperty(QObject *parent = 0);
    explicit DesignEqualsImplementationClassProperty(Type *propertyType, const QString &propertyName, Type *parentClassThatIamPropertyOf,  QObject *parent, bool hasInit, const QString &optionalInit, bool readOnly, bool notifiesOnChange);
    virtual QString typeString() { return PropertyType; }

    //TODOoptional: private + getter/setter blah
    QString PropertyType;
    QString PropertyName;
    bool ReadOnly;
    bool NotifiesOnChange;
    //TODOoptional: a few others, such as "reset" (which should be combined with "default"/initial-value imo)
};
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassProperty &designEqualsImplementationClassProperty);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassProperty &designEqualsImplementationClassProperty);
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassProperty *designEqualsImplementationClassProperty);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassProperty *designEqualsImplementationClassProperty);

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSPROPERTY_H
