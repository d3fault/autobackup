#ifndef LOCALBANKDB_H
#define LOCALBANKDB_H

#include <QObject>
#include <QHash>

#include "../sharedProtocol/userBankAccount.h"

//in-memory prototype, the impl will be db/persistent... and look into using a QCache if it fits
class LocalBankDb : public QObject
{
    Q_OBJECT
public:
    explicit LocalBankDb(QObject *parent = 0);
    void addUser(QString newUser);

    bool hasAddFundRequestBitcoinKey(QString user);
    bool addFundsRequestBitcoinKeyHasNoUpdates(QString user);
    QString getExistingAddFundsKey(QString user);
    bool addFundsRequestBitcoinKeyIsPendingConfirm(QString user);
    bool addFundsRequestBitcoinKeyIsConfirmed(QString user);
    void setFundsRequestBitcoinKey(QString user, QString newKey);
private:
          //name , details -- might be wiser to add .Name to UserBankAccount and keep it in a QList. idgaf
    QHash<QString, UserBankAccount*> m_Db;
signals:
    void d(const QString &);
public slots:

};

#endif // LOCALBANKDB_H
