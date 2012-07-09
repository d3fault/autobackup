#ifndef SSLTCPSERVERANDPROTOCOLKNOWER_H
#define SSLTCPSERVERANDPROTOCOLKNOWER_H

#include "irpcbankservermessagetransporter.h"

class SslTcpServerAndProtocolKnower : public IRpcBankServerMessageTransporter
{
    Q_OBJECT
public:
    explicit SslTcpServerAndProtocolKnower(QObject *mandatoryParent = 0);
private:
    SslTcpServer *m_Server;
signals:

public slots:

};

#endif // SSLTCPSERVERANDPROTOCOLKNOWER_H
