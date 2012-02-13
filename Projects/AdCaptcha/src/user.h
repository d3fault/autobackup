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

    //Wt::Dbo::collection< Wt::Dbo::ptr<Campaign> > m_Campaigns;

    int nothing;

    Wt::Dbo::collection< Wt::Dbo::ptr<UserAuthInfo> > authInfos;

    template<class Action /*lawsuit*/>
    void persist(Action &action)
    {
        //Wt::Dbo::field(action, m_Role, "role");
        Wt::Dbo::field(action, nothing, "nothing");

        //Wt::Dbo::hasMany(action, m_Campaigns, Wt::Dbo::ManyToOne, "user");
        Wt::Dbo::hasMany(action, authInfos, Wt::Dbo::ManyToOne, "user");
    }
};

#endif // USER_H
