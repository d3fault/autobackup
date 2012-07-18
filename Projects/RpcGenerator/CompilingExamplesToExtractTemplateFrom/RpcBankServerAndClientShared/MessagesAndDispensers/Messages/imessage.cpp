#include "imessage.h"

IMessage::IMessage(QObject *owner)
    : QObject(owner)
{ }
void IMessage::doneWithMessage()
{
    emit doneWithMessageSignal();
}
void IMessage::deliver()
{
    emit deliverSignal();
}
