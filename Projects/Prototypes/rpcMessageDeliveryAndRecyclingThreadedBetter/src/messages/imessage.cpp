#include "imessage.h"

IMessage::IMessage(QObject *mandatoryParent)
    : QObject(mandatoryParent)
{
}
void IMessage::doneWithMessage()
{
    emit doneWithMessageSignal();
}
void IMessage::deliver()
{
    emit deliverSignal();
}
