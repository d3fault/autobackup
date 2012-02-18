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

    //i say every user is BOTH by default.. and there is no choice to choose. less coding and WHO GIVES A FUCK
    //owner = the guy who puts the script/app on his site
    //publisher = the people who buy ad slots
#if 0
    enum Role
    {
        Owner = 0,
        Publisher = 1,
        Both = 2, //if i add more roles this is fucked.
        Admin = 3
    };
    Role m_Role;
#endif

    Wt::Dbo::collection< Wt::Dbo::ptr<SiteOwnerCampaign> > SiteOwnerCampaigns;
    Wt::Dbo::collection< Wt::Dbo::ptr<AdPublisherCampaign> > AdPublisherCampaigns;

    double CachedCredit; //todo: maybe use string to store more accurately??
    //cached because we don't want the server (which is offsite) to be the master db of who has how much etc. i don't trust sysadmins. our local tor/bitcoin server will verify transactions received (adding credits)... and verify cachecredit amount before transaction spending (buying ad slots with credits)/paying out (paying out credits to website owners)

    Wt::Dbo::collection< Wt::Dbo::ptr<UserAuthInfo> > authInfos;

    template<class Action /*lawsuit*/>
    void persist(Action &action)
    {
        //Wt::Dbo::field(action, m_Role, "role");
        Wt::Dbo::field(action, CachedCredit, "cachedcredit");

        Wt::Dbo::hasMany(action, SiteOwnerCampaigns, Wt::Dbo::ManyToOne, "user");
        Wt::Dbo::hasMany(action, AdPublisherCampaigns, Wt::Dbo::ManyToOne, "user");

        Wt::Dbo::hasMany(action, authInfos, Wt::Dbo::ManyToOne, "user");
    }
};

#endif // USER_H
