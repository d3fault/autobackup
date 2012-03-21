#ifndef USER_H
#define USER_H

#include <Wt/Dbo/Dbo>
#include <Wt/Auth/Dbo/AuthInfo>

class User;
typedef Wt::Auth::Dbo::AuthInfo<User> UserAuthInfo;

class User : public Wt::Dbo::Dbo<User>
{
public:
    User();

    enum AddFundStatuses
    {
        PaymentAwaiting = 0, //give em same key
        PaymentPending = 1, //say 'wait 10 mins'
        PaymentConfirmed = 2 //give em new key
    };

    Wt::Dbo::collection< Wt::Dbo::ptr<SiteOwnerCampaign> > SiteOwnerCampaigns;
    Wt::Dbo::collection< Wt::Dbo::ptr<AdPublisherCampaign> > AdPublisherCampaigns;

    double PendingBalance;
    double ConfirmedBalance;
    AddFundStatuses AddFundStatus;

    Wt::Dbo::collection< Wt::Dbo::ptr<UserAuthInfo> > authInfos;

    template<class Action>
    void persist(Action &action)
    {
        Wt::Dbo::field(action, AddFundStatus, "addfundstatus");
        Wt::Dbo::field(action, PendingBalance, "pendingbalance");
        Wt::Dbo::field(action, ConfirmedBalance, "confirmedbalance");

        Wt::Dbo::hasMany(action, SiteOwnerCampaigns, Wt::Dbo::ManyToOne, "user");
        Wt::Dbo::hasMany(action, AdPublisherCampaigns, Wt::Dbo::ManyToOne, "user");

        Wt::Dbo::hasMany(action, authInfos, Wt::Dbo::ManyToOne, "user");
    }
};

#endif // USER_H
