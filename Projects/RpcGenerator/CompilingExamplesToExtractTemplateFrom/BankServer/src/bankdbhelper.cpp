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
        createBankAccountMessage->deliver();
        return;
    }
    else
    {
        emit d(QString("BankDbHelper create bank account failed, username exists: ") + createBankAccountMessage->Username);
        createBankAccountMessage->createBankAccountFailedUsernameAlreadyExists();
        return;
    }
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
