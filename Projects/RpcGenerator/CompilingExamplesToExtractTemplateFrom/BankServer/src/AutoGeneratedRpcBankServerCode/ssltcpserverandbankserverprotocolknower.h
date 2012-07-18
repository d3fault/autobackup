#ifndef SSLTCPSERVERANDBANKSERVERPROTOCOLKNOWER_H
#define SSLTCPSERVERANDBANKSERVERPROTOCOLKNOWER_H

#include "ibankserverprotocolknower.h"

class SslTcpServerAndBankServerProtocolKnower : public IBankServerProtocolKnower
{
    Q_OBJECT
public:
    explicit SslTcpServerAndBankServerProtocolKnower(QObject *parent = 0);
};

#endif // SSLTCPSERVERANDBANKSERVERPROTOCOLKNOWER_H
