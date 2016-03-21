#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSPROPERTY_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSPROPERTY_H

#include "type.h"

class DesignEqualsImplementationClassProperty : public NonFunctionMember
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassProperty(Type *propertyType, const QString &qualifiedTypeString, const QString &propertyName, Type *parentClassThatIamPropertyOf,  QObject *parent, bool hasInit, const QString &optionalInit, bool readOnly, bool notifiesOnChange);

    QString propertyName() const;
    void setPropertyName(const QString &propertyName);

    bool ReadOnly;
    bool NotifiesOnChange;
    //TODOoptional: a few others, such as "reset" (which should be combined with "default"/initial-value imo)
private:
    QString m_PropertyName;
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSPROPERTY_H
