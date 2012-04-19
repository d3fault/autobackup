#include "appdbhelper.h"

AppDbHelper::AppDbHelper()
    //AFRd3abc: cachedUserAddFundsPageValues(1000) //TODOreq: research good cache max defaults. these must a) be big enough to justify having a cache... and b) small enough to not take up so much memory that Wt is limited in connections
    //for a maxCost for something such as cachedUserAddFundsPageValues, we could use the same #define that Wt uses for MaxConnections or something... or, better yet, MaxLogins. except there is no such value and it is simply a matter of available memory. still, whatever that number is is what we'd want this particular maxCost to be
    //for the maxCost of something AdSlot related, the number would probably be much lower... and the benefits of having a cache would be seen easier. it might even be wise to use a different caching strategy for those... QReadWriteLock (shared_lock boost).. but that presents it's own problems and i think even a race condition if i'm not mistaken?
{
    //don't do anything in here as the object hasn't been .moveToThread'd yet
}
AppDbHelper *AppDbHelper::m_pInstance = NULL;
AppDbHelper * AppDbHelper::Instance()
{
    if(!m_pInstance) //only one instance allowed
    {
        m_pInstance = new AppDbHelper();
    }
    return m_pInstance;
}
void AppDbHelper::init() //TODO: maybe a return value to inidicate success (even successful connection to AppDb (blockingQueued init THAT too))
{
    //TODOreq: set up thread for socket etc
}
#ifdef AFRd3abc
AddFundsRequestResult AppDbHelper::AddFundsRequest(QString username, CallbackInfo callbackInfo)
{

}
AppDbResult AppDbHelper::NowViewingAbcBalanceAddFunds(QString username, CallbackInfo callbackInfo)
{
    //TODO short term: when i comment this out to test the first log in shit, i should simplify this but still call it via BlockingQueued. return a random string and try to use it within Wt context. just to see if BlockingQueued even works...

    //TODO... re: generics. i could abstract the "AbcBalanceAddFunds" out of this maybe... and then call a virtual function for a list of values to notify for? another virtual function could contain the callback info...
    //for now, KISS

    /*if(userViewHash.contains(callbackInfo.clientPtr))
    {
        //i'm tempted to say REMOVE IT.. but seeing as this is a hash, merely inserting with the same key (clientPtr) will overwrite the previous value (the page they were at previously)... so this means we do nothing. commenting out.
    }*/
    userViewHash.insert(callbackInfo.clientPtr, callbackInfo.theClass); //theClass is of whatever that type is that we create the virtual function on. the callback will be callbackInfo.theClass.theCallback ... it is virtual too. if i need to i can do callbackInfo.theClassCallback. this might be necessary because i might need to bind it via "::" instead of "." and from within the class when we create the callbackInfo
    //TODOreq: when the session is destroyed, we need to remove our notification shit from the above hash
    //TODOreq: it might be better to use the username as the key for the above hash... because a notification from AppDb could come in and it might be a good optimization to say "ok what users does this data pertain to?" (for balances, we'd know exactly what user it would pertain to)... and it would save us from having to iterate the entire hash and checking if the values give a shit. idfk. my brain really hurts right now. HOWEVER multiple authenticated tabs would have the same username... and this would cause all kinds of illegal operations that i don't even want to begin thinking about. my brain hurts even more right now.

    //i can't wait till i get to the point where i can pay others to think for me..

    //maybe just insert the callbackInfo into the hash instead of the 'theClass' part of it. fuck man i'm a little lost here...


    AppDbResult result;

    result.NotInCacheWeWillNotifyYou = !cachedUserAddFundsPageValues.contains(username);

    if(result.NotInCacheWeWillNotifyYou)
    {
        return result;
    }

    UserAddFundsPageValues *pageValues = cachedUserAddFundsPageValues.object(username);

    result.ReturnString1 = pageValues->Key; //if it's [None] we still just pass it to the user. they then can request their first key. we check to see if the key is [None] before checking the AddFundStatus and give them their first key if it is. so somewhere in THIS class we must refer to an exact value of "[None]" so that it compares and equals to the same value that AppDb has set (although it can be blank, detect the blank, and THEN return "[None]"... we want what it returns to point to the same #define as what we compare to... just like in our RemoteBankManagementTest
    result.ReturnDouble1 = pageValues->PendingAmount;
    result.ReturnDouble2 = pageValues->ConfirmedKey;
    result.ReturnDouble3 = pageValues->AddFundsStatus; //0 = awaiting, 1 = pending, 2 = confirmed
    //may need to refactor this shit. a QDataStream might be better... but maybe not. KISS (even if coding too much) for now

    //check to see if in cache. if not, set bool appropriately and return. if is, fill out the rest and return
    //also set for notification of all updates to the values specified
    //i guess an sqlite db would be nice so i could deal with strings for what to check for etc... but idfk. i wish it had an in-memory cache like Wt... maybe it does...
    //i think i've decided NOT to use sql... so i'll just use memory for this cache. the reason being that it needs to be FAST... since this thread is called once per server 'GET'... which would result in 1 sql query (minimum) per 'GET'... and not even asynchronously... haha fuck that

    return result;
}
#endif
bool AppDbHelper::isBankAccountSetUpForUser(const std::string &usernameToCheck)
{
    //FUCK. false doesn't necessarily mean that a bank account isn't yet set up... it just means our cache doesn't have it. i guess maybe the wt-front-end can pull this entire list on initial connect (and of course receive notification updates)? TODOreq

    boost::shared_lock<boost::shared_mutex> sharedScopedLock(m_AlreadySetUpBankAccountsListMutex);
    if(m_AlreadySetUpBankAccountsList.find(usernameToCheck) != m_AlreadySetUpBankAccountsList.end())
    {
        return true;
    }
    return false;
}

//WE DON'T EVEN KNOW IF THE BLOCKINGQUEUED SHIT WILL WORK.. BUT I SEE NO REASON WHY IT WOULDN'T
//'we'
