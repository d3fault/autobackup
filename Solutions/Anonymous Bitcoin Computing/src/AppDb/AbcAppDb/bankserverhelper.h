#ifndef BANKSERVERHELPER_H
#define BANKSERVERHELPER_H

#include <QObject>

#include "Messages/bankserveractionrequest.h"
#include "Messages/bankserveractionrequestresponse.h"

class BankServerHelper : public QObject
{
    Q_OBJECT
public:
    explicit BankServerHelper(QObject *parent = 0);

signals:
    void d(const QString &);
    void connectedToBankServer();
    void responseToAppLogicReady(BankServerActionRequestResponse*);
public slots:
    void connectToBankServer();
    void handleBankServerActionRequest(BankServerActionRequest*);
};

#endif // BANKSERVERHELPER_H
