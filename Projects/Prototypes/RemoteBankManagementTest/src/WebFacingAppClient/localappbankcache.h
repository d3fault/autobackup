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
    LocalBankDb m_Db; //TODO: for now, in-memory works. but a future version of this prototype should use an sql db to be persistent across exe sessions.
signals:
    void userAdded(QString newUser);
    void d(const QString &);
public slots:
    void addUser(QString userNameToAdd);
    void handleUserAdded(QString newUser);
    void init();
};

#endif // LOCALAPPBANKCACHE_H
