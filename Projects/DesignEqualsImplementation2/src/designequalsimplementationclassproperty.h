#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSPROPERTY_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSPROPERTY_H

#include <QObject>

class DesignEqualsImplementationClassProperty : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassProperty(QObject *parent = 0);

    //TODOoptional: private + getter/setter blah
    QString Name;
    QString Type;
    bool ReadOnly;
    bool NotifiesOnChange;
    //TODOoptional: a few others, such as "reset" (which should be combined with "default"/initial-value imo)
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSPROPERTY_H
