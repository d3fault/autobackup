#ifndef SSLTCPSERVERANDBANKSERVERPROTOCOLKNOWER_H
#define SSLTCPSERVERANDBANKSERVERPROTOCOLKNOWER_H

#include "ibankserverprotocolknower.h"

class SslTcpServerAndBankServerProtocolKnower : public IBankServerProtocolKnower
{
    Q_OBJECT
public:
    explicit SslTcpServerAndBankServerProtocolKnower(QObject *parent = 0);
protected:
    void myTransmit(IMessage *message, uint uniqueRpcClientId);
};

#endif // SSLTCPSERVERANDBANKSERVERPROTOCOLKNOWER_H
