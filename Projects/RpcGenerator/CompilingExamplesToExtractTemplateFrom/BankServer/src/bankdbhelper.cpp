#include "bankdbhelper.h"

#include <QDateTime> //debug

BankDbHelper::BankDbHelper(QObject *parent)
    : QObject(parent)
{ }
void BankDbHelper::initialize(RpcBankServerClientsHelper *rpcBankServerClientsHelper)
{
    emit d("BankDbHelper received init message");

    connect(rpcBankServerClientsHelper, SIGNAL(createBankAccountRequested(CreateBankAccountMessage*)), this, SLOT(createBankAccount(CreateBankAccountMessage*)));
    connect(rpcBankServerClientsHelper, SIGNAL(getAddFundsKeyRequested(GetAddFundsKeyMessage*)), this, SLOT(getAddFundsKey(GetAddFundsKeyMessage*)));

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
void BankDbHelper::createBankAccount(CreateBankAccountMessage *createBankAccountMessage)
{
    /*
    THIS METHOD IS CURRENT, BUT THE BACKING CODE IS NOT YET THERE. WE ARE CHANGING FROM THE ABOVE #ifdef'd out code above...
            WHITELIST PROGRAMMING FTW
            ALSO, the getAddFundsKey method below needs to be converted to whitelist programming mode...
            need to:
                [x] make failed methods use flagging instead of acting like deliver
                [ ] make deliver handle failed flags
    */

        //TODO LEFT OFF -- pretty important before trying to test.. but not that difficult, even though i still dunno where it goes lmfao:
        //TODOreq: not sure where it goes exactly, but need to set Header.Success to false just before the dispenser dispenses it. we don't want to keep the old state of a recycled message.... but i mean this is a TODOreq for all of the properties of the messages, not just the success boolean. Here's an idea for it: a virtual void resetParameters() that is implemented at the lowest level (IRecycleable?), and overwritten CALLING THE PARENT resetting all the child parameters as necessary. Test/Verify: a base non-cast'd pointer calling a virtual method will execute the most-recently-overwritten virtual instead. PRETTY SURE this is true.. but it's a little more dangerous than having a pure virtual because if I forget to override it in my child, I don't get errors. Or just a pure virtual implemented where relevant. idgaf

    if(!m_AllUsers.contains(createBankAccountMessage->Username))
    {
        m_AllUsers.append(createBankAccountMessage->Username);
        if(!m_AllUsers.contains(createBankAccountMessage->Username)) //this will never happen, but might in production environment. need to handle persist errors
        {
            //createBankAccountMessage->m_ErrorCode = CreateBankAccountMessage::FailedUsernameAlreadyExists;
            //vs.
            createBankAccountMessage->setFailedPersistError(); //which will do the same thing internally... (NOTE, it used to not. it used to be hooked up just like deliver() was). if i do this, they'd need to be #ifdef'd out depending on if server/client. we only need it on the server..
            //the first is faster because it has the same amount of derefs and is just a simple assign operation, whereas jumps are a lot more expensive (inlining solves!)
            //the latter also lets the user type in 'fail' in their auto-complete to see all the options... the second doesn't let them do that until you get to the enum choosing part. so fuck it, choose the one with less code especially since you can just inline that shit to make it the exact same fucking code. less code to type for users = woot
        }
        else
        {
            emit d(QString("BankDbHelper created bank account for user: ") + createBankAccountMessage->Username);
            createBankAccountMessage->setSuccessful();
        }
    }
    else
    {
        emit d(QString("BankDbHelper create bank account failed, username exists: ") + createBankAccountMessage->Username);
        createBankAccountMessage->setFailedUsernameAlreadyExists();
    }
    createBankAccountMessage->deliver();
}
void BankDbHelper::getAddFundsKey(GetAddFundsKeyMessage *getAddFundsKeyMessage)
{
    if(m_AllUsers.contains(getAddFundsKeyMessage->Username))
    {
        //TODOreq: add to db etc, perhaps relay to bitcoin? handle other error cases too... which is why i need to be in bitcoin lol. i'm thinking we should send it to bitcoin FIRST, then have him relay his results to us... and then we call deliver after adding it to the db. I'm not really sure about though... need to figure out precisely how I'm going to store bitcoin user data and where (TODOreq: another thing I've been thinking about is each 'service' server will have it's own bitcoindb.db file, so later accesses need to get back to the right service or we might find that the user doesn't have an account in the bitcoindb.db on that particular rpc service server. one way to solve this is to just use global non-user-specific bitcoindb.db files and then to store all the information in the couchbase cluster. this will lead to weird balancing issues, and we might even have to shift around bitcoins from rpc-server to rpc-server in order to have enough for a payout etc. i'm not sure about this altogether but a solution doesn't sound too difficult. obviously if i get better at bitcoin (use a library instead of the binary directly) then I can just store all the values in the couchbase cluster like normal...)

        //since this is all "user" code in relation to Rpc Generator, I should ignore it for now. Just wanna get Rpc up and running and then I can do this shit later when I have couchbase up and running :-P

        //debug:
        getAddFundsKeyMessage->AddFundsKey = QDateTime::currentDateTime().toString();
        emit d(QString("BankDb got add funds key: '") + getAddFundsKeyMessage->AddFundsKey + QString("' for user: " ) + getAddFundsKeyMessage->Username); //TODOoptional: a message->setReturnParameters(QString addFundsKey, etc, etc); might be handy... as would multiple singular setReturnParameter(QString addFundsKey); functions... but meh, semantics + helpers for users idgaf. the multi-parameter one could also setSuccessful(), so the user doesn't have to. it does look cleaner i guess. but then what if they don't have all the parameters at once (especially if we relay the message to another class/thread)? the singular ones would become necessary, and then the user would not be in the habit of calling setSuccessful... so would probably omit it unintentionally? a hacky solution to that would be to see if, when setting any of the singular parameters, all the other parameters have been set. i don't like that though because it adds stupid overhead
        getAddFundsKeyMessage->setSuccessful();
    }
    else
    {
        emit d(QString("BankDb get add funds key failed, username doesn't exist: ") + getAddFundsKeyMessage->Username);
        getAddFundsKeyMessage->setFailedUsernameDoesntExist();
    }
    getAddFundsKeyMessage->deliver();
}
