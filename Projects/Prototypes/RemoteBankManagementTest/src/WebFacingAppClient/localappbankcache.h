#ifndef LOCALAPPBANKCACHE_H
#define LOCALAPPBANKCACHE_H

#include <QObject>

#include "localbankdb.h"
#include "remotebankserverhelper.h"

class LocalAppBankCache : public QObject
{
    Q_OBJECT
public:
    explicit LocalAppBankCache(QObject *parent = 0);
private:
    RemoteBankServerHelper *m_BankServer; //helper, encapsulates ssl + protocol etc
    LocalBankDb m_Db; //TODO: for now, in-memory works. but a future version of this prototype should use an sql db to be persistent across exe sessions. maybe this should be a pointer initialized in the init() slot? so that the thread that we moveTo owns it? i really don't know if it matters :-/... what the fuck are "child objects". moveToThread also moves all "child objects"... are those child widgets? or any QObject that has this passed into it's constructor for *parent?? as of now i know one thing: the db is being constructed on the GUI thread.

    void userNeedsNewBitcoinAddFundsKey(QString user);
signals:
    void userAdded(QString newUser);
    //void addFundsKeyReceived(QString user, QString newKey);
    void d(const QString &);
public slots:
    //handle requests from the gui/user
    void addUser(QString userNameToAdd);
    void addFundsRequest(QString user);

    //handle responses from the server
    void handleUserAdded(QString newUser);
    void handleAddFundsKeyReceived(QString user, QString newKey);
    void handlePendingPaymentReceived(QString user, QString key, double pendingAmount);
    void handleConfirmedPaymentReceived(QString user, QString key, double confirmedAmount);
    void init();
};

#endif // LOCALAPPBANKCACHE_H
