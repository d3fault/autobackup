#ifndef USERNAMEDB_H
#define USERNAMEDB_H

#include <Wt/Dbo/Dbo>
#include <Wt/Auth/Dbo/AuthInfo>

class UsernameDb;
typedef Wt::Auth::Dbo::AuthInfo<UsernameDb> UserAuthInfo;

class UsernameDb : public Wt::Dbo::Dbo<UsernameDb>
{
public:
    UsernameDb();

    Wt::Dbo::collection< Wt::Dbo::ptr<UserAuthInfo> > authInfos;

    template<class Action>
    void persist(Action &action)
    {

        Wt::Dbo::hasMany(action, authInfos, Wt::Dbo::ManyToOne, "usernamedb");
    }
};

DBO_EXTERN_TEMPLATES(UsernameDb);

#endif // USERNAMEDB_H
