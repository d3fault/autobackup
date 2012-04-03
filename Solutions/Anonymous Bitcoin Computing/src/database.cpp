#include "database.h"

//anon namespace makes the contents only accessible from within this file
namespace
{
    //these can't be member variables because we can't rely on Database to be instantiated even once before we call GlobalConfigure
    Wt::Auth::AuthService s_AuthService;
    Wt::Auth::PasswordService s_PasswordService(s_AuthService);
}

Database::Database()
    : m_Sqlite(Wt::WApplication::instance()->appRoot() + "usernamedb.db") /*not sure if i can make this scalable by splitting the username a-z thing in half... since it's so tied in with Wt*/
{
    //todo: a ReadWriteLock equivalent (in boost??) (i only know of it in Qt) of an in-memory cache of an array of AdCampaigns
    //it should have a dirty bit that is set to true whenever we write... or i guess we could just update the cache on write...
    //still, it's stupid to do a transaction for every time we do a READ in WidgetSet mode to get campaign details and/or ad imagery (cache of MOST-USED images would be nice too... capped at memory limit). we just want to limit hdd reads is all... the master copies are of course still saved
    //basically a custom in-app implementation of memcached (research that first to see if it could do it for us... not that it'd even be hard to make to begin with)

    m_DbSession.setConnection(m_Sqlite);
    m_Sqlite.setProperty("show-queries", "false");
    //todo: maybe enable WAL (write-ahead-logging (i don't know that that is what setProperty accepts, research this)) for sqlite... enables simultaneous writes and error recovery if crash during transaction. idk if it's needed... but it sounds interesting

    m_DbSession.mapClass<UsernameDb>("usernamedb");
    m_DbSession.mapClass<UserAuthInfo>("auth_info");
    m_DbSession.mapClass<UserAuthInfo::AuthIdentityType>("auth_identity");
    m_DbSession.mapClass<UserAuthInfo::AuthTokenType>("auth_token");

    m_UserDb = new UserDatabase(m_DbSession);


    Wt::Dbo::Transaction transaction(m_DbSession);
    try
    {
        m_DbSession.createTables();
        //todo: maybe set up an admin account here? if you do, make sure you change the password on first login as the password would be shown here in plaintext
    }
    catch(...)
    {
        //table already created. weird that this is how they prefer to do it... but i'm not about to fight it
    }
    transaction.commit();
}
void Database::GlobalConfigure()
{
    //TODOreq: Database is one-per-server... which means that technicall we can access it from the WidgetSet (to be created)... but we don't want or need any of the authentication functionality in the WidgetSet. i don't like the idea of having 2 servers. they will both read from the database... but only the site portion of it will write (and therefore need authentication)
    //TODOreq: extending on the above todo, what will happen when a user with an auth token requests a WidgetSet? his cookies are sent to the server... but are they processed? I think not, they are only processed by authWidget->processEnvironment();.... which is in the WContainerWidget implmentation (i'll only put it in the site's one... not the embedded/WidgetSet's one)... so while yes, the cookies will be sent (as per the functionality of cookies... if you're at the domain.. they get sent (can't i make it so they are only sent when i'm at a certain path?))

    s_AuthService.setAuthTokensEnabled(true, "adcaptcha");
    s_AuthService.setEmailVerificationEnabled(true);

    Wt::Auth::PasswordVerifier *passwordVerifier = new Wt::Auth::PasswordVerifier(); //todo... the reference material does NOT delete this pointer (it does use pointers, unlike authservice (scoped deletion))... so does .setVerifier take ownership of it and delete it for me??
    passwordVerifier->addHashFunction(new Wt::Auth::BCryptHashFunction(7));
    s_PasswordService.setVerifier(passwordVerifier);

    s_PasswordService.setStrengthValidator(new Wt::Auth::PasswordStrengthValidator());
    s_PasswordService.setAttemptThrottlingEnabled(true);
}
const Wt::Auth::AuthService &Database::getAuthService()
{
    return s_AuthService;
}
const Wt::Auth::AbstractPasswordService & Database::getPasswordService()
{
    return s_PasswordService;
}
Wt::Auth::Login &Database::getLogin()
{
    return m_Login;
}
Wt::Auth::AbstractUserDatabase & Database::getUserDatabase()
{
    return *m_UserDb;
}
Database::~Database()
{
    delete m_UserDb;
}
