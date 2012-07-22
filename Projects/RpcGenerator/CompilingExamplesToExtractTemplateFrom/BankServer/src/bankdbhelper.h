#ifndef BANKDBHELPER_H
#define BANKDBHELPER_H

#include <QObject>
#include <QList>

#include "../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessage.h"
#include "../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessage.h"

class BankDbHelper : public QObject
{
    Q_OBJECT
public:
    explicit BankDbHelper();
private:
    QList<QString> m_AllUsers; //my actual db for testing lol
signals:
    void initialized();
    void started();
    void stopped();
public slots:
    void init();
    void start();
    void stop();

    void createBankAccount(CreateBankAccountMessage *createBankAccountMessage);
    void getAddFundsKey(GetAddFundsKeyMessage *getAddFundsKeyMessage);
};

#endif // BANKDBHELPER_H
