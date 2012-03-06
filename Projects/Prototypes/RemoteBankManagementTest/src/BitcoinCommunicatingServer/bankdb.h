#ifndef BANKDB_H
#define BANKDB_H

#include <QObject>
#include <QHash>

#include "../sharedProtocol/userBankAccount.h"

//in-memory prototype. impl will be database backed/persistent
class BankDb : public QObject
{
    Q_OBJECT
public:
    explicit BankDb(QObject *parent = 0);
    void addUser(const QString &appId, const QString &userName); //todo: should either return a bool or an AddUserResult. to make this initial prototype easier and to get it up and running, we're going to assume it worked... because it will
    void setAddFundsKey(const QString &appId, const QString &userName, const QString &newKey);
private:
    /*
      --- our 'local app bank db' reciprocal ---
      we need to take into account the appId
          //user , balance
    QHash<QString, qint64> m_Db;
    */

          //appId        //user   //details
    QHash<QString, QHash<QString, UserBankAccount*>* > m_Db;
signals:
    void d(const QString &);
};

#endif // BANKDB_H
