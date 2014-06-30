#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSMETHODARGUMENT_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSMETHODARGUMENT_H

#include <QObject>

class DesignEqualsImplementationClassMethodArgument : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassMethodArgument(QObject *parent = 0);

    //TODOoptional: private + getter/setter blah
    QString Type;
    QString Name;
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSMETHODARGUMENT_H
