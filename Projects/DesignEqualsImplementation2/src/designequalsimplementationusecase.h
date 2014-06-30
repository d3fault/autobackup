#ifndef DESIGNEQUALSIMPLEMENTATIONUSECASE_H
#define DESIGNEQUALSIMPLEMENTATIONUSECASE_H

#include <QObject>
#include <QList>

#include "designequalsimplementationusecaseevent.h"

class DesignEqualsImplementationUseCase : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationUseCase(QObject *parent = 0);
    ~DesignEqualsImplementationUseCase();

    //TODOoptional: private + getter/setter blah
    QList<DesignEqualsImplementationUseCaseEvent*> OrderedUseCaseEvents;
};
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationUseCase &useCase);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationUseCase &useCase);

#endif // DESIGNEQUALSIMPLEMENTATIONUSECASE_H
