#ifndef BANKSERVERCLIENT_H
#define BANKSERVERCLIENT_H

#include "AutoGeneratedRpcBankServerClientCode/irpcbankserverclientbusiness.h"

class BankServerClient : public IRpcBankServerClientBusiness
{
    Q_OBJECT
public:
    BankServerClient();
    void instructBackendObjectsToClaimRelevantDispensers();
    void moveBackendBusinessObjectsToTheirOwnThreadsAndStartTheThreads();
public slots:
    void init();
    void start();
    void stop();
};

#endif // BANKSERVERCLIENT_H
