#ifndef BANKSERVERCLIENTTEST_H
#define BANKSERVERCLIENTTEST_H

#include "idebuggablestartablestoppablebackend.h"

#include "bankserverclient.h"

class BankServerClientTest : public IDebuggableStartableStoppableBackend
{
    Q_OBJECT
public:
    BankServerClientTest();
private:
    BankServerClient *m_BankClient;

public slots:
    void init();
    void start();
    void stop();

    void simulateCreateBankAccountAction();
    void simulateGetAddFundsKeyAction();
};

#endif // BANKSERVERCLIENTTEST_H
