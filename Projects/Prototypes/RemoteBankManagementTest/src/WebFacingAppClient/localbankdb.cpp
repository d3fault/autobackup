#include "localbankdb.h"

LocalBankDb::LocalBankDb(QObject *parent) :
    QObject(parent)
{ }
void LocalBankDb::addUser(QString newUser)
{
    m_Db.insert(newUser, 0.0);
}
