#include "designequalsimplementationusecase.h"

#define DesignEqualsImplementationUseCase_QDS(direction, qds, useCase) //TODOreq

//TODOreq: by far the most difficult class to design/[de-]serialize/generate-from. the rest are just busywork by comparison
DesignEqualsImplementationUseCase::DesignEqualsImplementationUseCase(QObject *parent)
    : QObject(parent)
{ }
DesignEqualsImplementationUseCase::~DesignEqualsImplementationUseCase()
{
    qDeleteAll(OrderedUseCaseEvents);
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationUseCase &useCase)
{
    DesignEqualsImplementationUseCase_QDS(<<, out, useCase)
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationUseCase &useCase)
{
    DesignEqualsImplementationUseCase_QDS(>>, in, useCase)
}
