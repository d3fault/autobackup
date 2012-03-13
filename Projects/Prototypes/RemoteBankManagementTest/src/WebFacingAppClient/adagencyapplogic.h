#ifndef ADAGENCYAPPLOGIC_H
#define ADAGENCYAPPLOGIC_H

#include <QObject>

#include "appuserdb.h"

class AdAgencyAppLogic : public QObject
{
    Q_OBJECT
public:
    explicit AdAgencyAppLogic(QObject *parent = 0);
private:
    AppUserDb *m_Db;
signals:
    void campaignAdded(QString adCampaignName);
    void d(const QString &);
public slots:
    void init();
    void addUser(QString newUser);
    void createAdCampaignXForUserY(QString adCampaignName, QString adCampaignOwnerName);
    void purchaseSlotXforUserY(QString adCampaignNameToPurchaseSlotFrom, QString slotPurchaserUsername);
};

#endif // ADAGENCYAPPLOGIC_H
