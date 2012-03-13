#ifndef APPUSERDB_H
#define APPUSERDB_H

#include <QObject>
#include <QHash>

#include "AppUserAccount.h"

class AppUserDb : public QObject
{
    Q_OBJECT
public:
    explicit AppUserDb(QObject *parent = 0);
    void addUser(QString newUser);
    void addCampaign(QString adCampaignOwnerName, QString adCampaignName);
private:
    QHash<QString, AppUserAccount*> m_Db;
signals:
    void d(const QString &);
public slots:

};

#endif // APPUSERDB_H
