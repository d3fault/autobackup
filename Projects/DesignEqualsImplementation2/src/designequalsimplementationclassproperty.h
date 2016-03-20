#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSPROPERTY_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSPROPERTY_H

#include "type.h"

class DesignEqualsImplementationClassProperty : public NonFunctionMember
{
    Q_OBJECT
public:
    //explicit DesignEqualsImplementationClassProperty(QObject *parent = 0);
    explicit DesignEqualsImplementationClassProperty(Type *propertyType, const QString &propertyName, Type *parentClassThatIamPropertyOf,  QObject *parent, bool hasInit, const QString &optionalInit, bool readOnly, bool notifiesOnChange);

    QString PropertyName; //i'm hesitant to string-replace this ("->PropertyName") with "->VariableName"), because I might want to keep the 'property name' sans m_ somewhere, and I haven't clearly defined (to myself or otherwise) what VariableName even means in the context of a Q_PROPERTY
    bool ReadOnly;
    bool NotifiesOnChange;
    //TODOoptional: a few others, such as "reset" (which should be combined with "default"/initial-value imo)
};
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassProperty &designEqualsImplementationClassProperty);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassProperty &designEqualsImplementationClassProperty);
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassProperty *designEqualsImplementationClassProperty);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassProperty *designEqualsImplementationClassProperty);

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSPROPERTY_H
