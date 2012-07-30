#include "bankdbhelper.h"

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
    emit d(QString("BankDbHelper received createBankAccountMessage with user: ") + createBankAccountMessage->Username);
    if(!m_AllUsers.contains(createBankAccountMessage->Username))
    {
        m_AllUsers.append(createBankAccountMessage->Username);
        if(!m_AllUsers.contains(createBankAccountMessage->Username)) //this will never happen, but might in production environment. need to handle persist errors
        {
            createBankAccountMessage->createBankAccountFailedPersistError();
            return;
        }
        createBankAccountMessage->deliver();
        return;
    }
    else
    {
        //TODOreq: errors n shit
        createBankAccountMessage->createBankAccountFailedUsernameAlreadyExists();
        return;
    }
}
void BankDbHelper::getAddFundsKey(ServerGetAddFundsKeyMessage *getAddFundsKeyMessage)
{
    emit d(QString("BankDbHelper received getAddFundsKeyMessage with user: ") + getAddFundsKeyMessage->Username);
    if(!m_AllUsers.contains(getAddFundsKeyMessage->Username))
    {
        //TODOreq: user not exist error
        //getAddFundsKeyMessage->get
    }
    else
    {
        //TODOreq: add to db etc, perhaps relay to bitcoin?
        getAddFundsKeyMessage->deliver();
    }
}
