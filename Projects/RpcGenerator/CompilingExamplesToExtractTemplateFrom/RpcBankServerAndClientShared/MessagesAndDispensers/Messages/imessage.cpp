#include "imessage.h"

IMessage::IMessage(QObject *owner)
    : IRecycleableAndStreamable(owner)
{ }
void IMessage::deliver()
{
    emit deliverSignal();
}
