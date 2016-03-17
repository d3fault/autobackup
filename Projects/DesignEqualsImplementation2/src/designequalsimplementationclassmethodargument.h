#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSMETHODARGUMENT_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSMETHODARGUMENT_H

#include "type.h"
typedef TypeInstance DesignEqualsImplementationClassMethodArgument; //the typedef is still useful because it helps readability/understandbility. Not all TypeInstances are Arguments, but all Arguments are TypeInstances. I should change the name to d=iFunctionArgument TODOoptional

#if 0
class DesignEqualsImplementationClassMethodArgument : public TypeInstance
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassMethodArgument(QObject *parent = 0);
    explicit DesignEqualsImplementationClassMethodArgument(const QString &variableName, QObject *parent = 0);
    virtual ~DesignEqualsImplementationClassMethodArgument();

    virtual QString typeString();

    //TODOoptional: private + getter/setter blah
    QString Type;
};
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassMethodArgument &argument);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassMethodArgument &argument);
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassMethodArgument *argument);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassMethodArgument *argument);

#endif
#endif // DESIGNEQUALSIMPLEMENTATIONCLASSMETHODARGUMENT_H
