#include "irecycleableandstreamable.h"

IRecycleableAndStreamable::IRecycleableAndStreamable(QObject *parent) :
    QObject(parent)
{ }
void IRecycleableAndStreamable::doneWithMessage()
{
    emit doneWithMessageSignal();
}
