#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSMETHODARGUMENT_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSMETHODARGUMENT_H

#include <QObject>
#include "ihavetypeandvariablenameandpreferredtextualrepresentation.h"

class DesignEqualsImplementationClassMethodArgument : public QObject, public IHaveTypeAndVariableNameAndPreferredTextualRepresentation
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassMethodArgument(QObject *parent = 0);
    virtual ~DesignEqualsImplementationClassMethodArgument();

    virtual QString typeString();

    //TODOoptional: private + getter/setter blah
    QString Type;
};
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassMethodArgument &argument);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassMethodArgument &argument);
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassMethodArgument *&argument);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassMethodArgument *&argument);

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSMETHODARGUMENT_H
