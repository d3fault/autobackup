#include "appdbhelper.h"

AppDbHelper::AppDbHelper()
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
AppDbResult AppDbHelper::AddFundsRequest(QString username, CallbackInfo callbackInfo)
{
    AppDbResult result;
    //check to see if in cache. if not, set bool appropriately and return. if is, fill out the rest and return
    //also set for notification of all updates to the values specified
    return result;
}




//WE DON'T EVEN KNOW IF THE BLOCKINGQUEUED SHIT WILL WORK.. BUT I SEE NO REASON WHY IT WOULDN'T
//'we'
