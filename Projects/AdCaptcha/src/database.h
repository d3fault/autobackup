#ifndef DATABASE_H
#define DATABASE_H

#include <Wt/Auth/AuthService>
#include <Wt/Auth/PasswordService>
#include <Wt/Auth/PasswordVerifier>
#include <Wt/Auth/PasswordStrengthValidator>
#include <Wt/Auth/Login>
#include <Wt/Dbo/backend/Sqlite3>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/Transaction>

#include "user.h"

class Database
{
public:
    Database();
    static void GlobalConfigure(); //called once per server instance.. not once per client connection
    static const Wt::Auth::AuthService &getAuthService();
    static const Wt::Auth::AbstractPasswordService &getPasswordService();
    Wt::Auth::Login &getLogin();
private:
    Wt::Auth::Login m_Login;
    Wt::Dbo::backend::Sqlite3 m_Sqlite;
    mutable Wt::Dbo::Session m_DbSession;
    Wt::Auth::Dbo::UserDatabase<Wt::Auth::Dbo::AuthInfo<User> > *m_UserDb;
};

#endif // DATABASE_H
