#include "bankdb.h"

BankDb::BankDb(QObject *parent) :
    QObject(parent)
{
}
void BankDb::addUser(const QString &appId, const QString &userName)
{
    QString nonConstAppId(appId);
    if(m_Db.count() > 0 && m_Db.contains(nonConstAppId))
    {
        //todo: check that username doesn't already exist
        QHash<QString, double> listOfAccountsForThisAppId = m_Db.value(nonConstAppId);
        //todo: check that this doesn't extract a hardcopy that is then modified... that we're in fact modifying the list that lives in m_Db
        listOfAccountsForThisAppId.insert(userName, 0.0);
    }
    else
    {
        //since appId doesn't exist, we know username doesn't already exist
        QHash<QString, double> newListOfAccountsForThisAppId; //new because the list doesn't yet exist since the appId doesn't yet exist
        newListOfAccountsForThisAppId.insert(userName, 0.0);
        m_Db.insert(nonConstAppId, newListOfAccountsForThisAppId);
    }
}
