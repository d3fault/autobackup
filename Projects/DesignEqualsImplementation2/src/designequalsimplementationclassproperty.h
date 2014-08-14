#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSPROPERTY_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSPROPERTY_H

#include <QObject>

class DesignEqualsImplementationClassProperty : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassProperty(QObject *parent = 0);
    explicit DesignEqualsImplementationClassProperty(const QString &propertyType, const QString &propertyName, bool readOnly, bool notifiesOnChange, QObject *parent = 0);

    //TODOoptional: private + getter/setter blah
    QString Type;
    QString Name;
    bool ReadOnly;
    bool NotifiesOnChange;
    //TODOoptional: a few others, such as "reset" (which should be combined with "default"/initial-value imo)
};
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassProperty &designEqualsImplementationClassProperty);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassProperty &designEqualsImplementationClassProperty);
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassProperty *&designEqualsImplementationClassProperty);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassProperty *&designEqualsImplementationClassProperty);

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSPROPERTY_H
