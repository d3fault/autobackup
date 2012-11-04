#include "iactionmessage.h"

IActionMessage::IActionMessage(QObject *parent) :
    IMessage(parent), m_ConnectionIndependentClientId(0)
{
    resetSuccessAndErrorCode();
}
