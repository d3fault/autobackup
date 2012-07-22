#include "bankdbhelper.h"

BankDbHelper::BankDbHelper()
{
}
void BankDbHelper::init()
{
    emit initialized();
}
void BankDbHelper::start()
{
    emit started();
}
void BankDbHelper::stop()
{
    emit stopped();
}
void BankDbHelper::createBankAccount(CreateBankAccountMessage *createBankAccountMessage)
{
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
void BankDbHelper::getAddFundsKey(GetAddFundsKeyMessage *getAddFundsKeyMessage)
{
}
