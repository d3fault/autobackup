#include "localbankdb.h"

LocalBankDb::LocalBankDb(QObject *parent) :
    QObject(parent)
{
    m_Db = new QHash<QString,double>();
}
void LocalBankDb::addUser(QString newUser)
{
    m_Db->insert(newUser, 0.0);
}
