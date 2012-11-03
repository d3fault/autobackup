#include "iactionmessage.h"

IActionMessage::IActionMessage(QObject *parent) :
    IMessage(parent), m_ToggleBit(false)
{
    resetSuccessAndErrorCode();
}
