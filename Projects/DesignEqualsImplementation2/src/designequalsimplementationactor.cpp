#include "designequalsimplementationactor.h"

DesignEqualsImplementationActor::DesignEqualsImplementationActor(QPointF position, QObject *parent)
    : QObject(parent)
    , m_Position(position)
{ }
QPointF DesignEqualsImplementationActor::position() const
{
    return m_Position;
}
