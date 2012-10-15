#include "bankdbhelper.h"

#include <QDateTime> //debug

BankDbHelper::BankDbHelper()
{
}
void BankDbHelper::init()
{
    emit d("BankDbHelper received init message");
    emit initialized();
}
void BankDbHelper::start()
{
    emit d("BankDbHelper received start message");
    emit started();
}
void BankDbHelper::stop()
{
    emit d("BankDbHelper received stop message");
    emit stopped();
}
void BankDbHelper::createBankAccount(ServerCreateBankAccountMessage *createBankAccountMessage)
{
#if 0
    if(!m_AllUsers.contains(createBankAccountMessage->Username))
    {
        m_AllUsers.append(createBankAccountMessage->Username);
        if(!m_AllUsers.contains(createBankAccountMessage->Username)) //this will never happen, but might in production environment. need to handle persist errors
        {
            createBankAccountMessage->createBankAccountFailedPersistError();
            return;
        }
        else
        {
            emit d(QString("BankDbHelper created bank account for user: ") + createBankAccountMessage->Username);
        }
        createBankAccountMessage->deliver(); //TODOreq: the overall flow of code for these slots should be re-written/designed so that instead of defaulting to success unless a failed* is called, a genericFailed* is called by default. the deliver() has to be opted into (whitelist programming), and the errors are handled wherever detected just like in the current code...
        return;
    }
    else
    {
        emit d(QString("BankDbHelper create bank account failed, username exists: ") + createBankAccountMessage->Username);
        createBankAccountMessage->createBankAccountFailedUsernameAlreadyExists();
        return;
    }
#endif

    THIS METHOD IS CURRENT, BUT THE BACKING CODE IS NOT YET THERE. WE ARE CHANGING FROM THE ABOVE #ifdef'd out code above...
            WHITELIST PROGRAMMING FTW
            ALSO, the getAddFundsKey method below needs to be converted to whitelist programming mode...
            need to:
                make failed methods use flagging instead of acting like deliver
                make deliver handle failed flags

        //TODOreq: not sure where it goes exactly, but need to set Header.Success to false just before the dispenser dispenses it. we don't want to keep the old state of a recycled message.... but i mean this is a TODOreq for all of the properties of the messages, not just the success boolean

    if(!m_AllUsers.contains(createBankAccountMessage->Username))
    {
        m_AllUsers.append(createBankAccountMessage->Username);
        if(!m_AllUsers.contains(createBankAccountMessage->Username)) //this will never happen, but might in production environment. need to handle persist errors
        {
            createBankAccountMessage->createBankAccountFailedPersistError();
        }
        else
        {
            emit d(QString("BankDbHelper created bank account for user: ") + createBankAccountMessage->Username);
            createBankAccountMessage->Header.Success = true; //TODOoptional: instead of setting a bool, just a success(); function would do (to align with the failed methods. doesn't matter much~. in fact, setting a bool might even be faster... but idk mb not with dereferencing. fuck these premature optimizations, get out of my brain!!!!)
        }
    }
    else
    {
        emit d(QString("BankDbHelper create bank account failed, username exists: ") + createBankAccountMessage->Username);
        createBankAccountMessage->createBankAccountFailedUsernameAlreadyExists();
    }

    //TODOreq: change functionality of the failed methods so that they only set up flagging (and i guess allow multiple failed reasons? TODOoptional that one is). I want deliver(); to be used _ALWAYS_. if you call it without setting Success to true, we returna generic error message

    createBankAccountMessage->deliver();
}
void BankDbHelper::getAddFundsKey(ServerGetAddFundsKeyMessage *getAddFundsKeyMessage)
{
    if(!m_AllUsers.contains(getAddFundsKeyMessage->Username))
    {
        emit d(QString("BankDb get add funds key failed, username doesn't exist: ") + getAddFundsKeyMessage->Username);
        getAddFundsKeyMessage->getAddFundsKeyFailedUsernameDoesntExist();
    }
    else
    {
        //TODOreq: add to db etc, perhaps relay to bitcoin?
        //debug:
        getAddFundsKeyMessage->AddFundsKey = QDateTime::currentDateTime().toString();
        emit d(QString("BankDb got add funds key: '") + getAddFundsKeyMessage->AddFundsKey + QString("' for user: " ) + getAddFundsKeyMessage->Username);

        getAddFundsKeyMessage->deliver();
    }
}
