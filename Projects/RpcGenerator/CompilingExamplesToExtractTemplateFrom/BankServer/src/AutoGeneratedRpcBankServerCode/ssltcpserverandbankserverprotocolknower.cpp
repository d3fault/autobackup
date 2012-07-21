#include "ssltcpserverandbankserverprotocolknower.h"

SslTcpServerAndBankServerProtocolKnower::SslTcpServerAndBankServerProtocolKnower(QObject *parent)
    : IBankServerProtocolKnower(parent)
{
}
//Action Requests
//in some handleMessageReceivedFromClient method, we read the header and use our message dispensers to get and emit the right message type

//Action Responses + Broadcasts
void SslTcpServerAndBankServerProtocolKnower::myTransmit(IMessage *message, uint uniqueRpcClientId)
{
    //TODO -- uniqueRpcClientId is 0 for broadcasts... and perhaps if the action broadcasts it's response?
}
