#ifndef BANKDBHELPER_H
#define BANKDBHELPER_H

#include <QObject>

#include "couchbaseclusterdbhelper.h"

//the main benefit of having this class is that the actual storage engine and the technique for accessing it is abstracted from the Bank. making a new BankDbHelper (at which point i could create an interface between this BankDbHelper and the new one) using a different back-end is trivial

class BankDbHelper : public QObject
{
    Q_OBJECT
public:
    explicit BankDbHelper(QObject *parent = 0);
private:
    CouchBaseClusterDbHelper *m_Db; //BankDbHelper (us) gets a it's own class (did i mean thread here?), but CouchBaseClusterDbHelper does not [need one]. calls to it our synchronous between us, as they should be
signals:
    void initialized();
public slots:
    void init();
    void createBankAccount(const QString &);
};

#endif // BANKDBHELPER_H
