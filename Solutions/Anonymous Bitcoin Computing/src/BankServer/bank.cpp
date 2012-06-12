#include "bank.h"

void Bank::init()
{
    //todo: init sql db i guess. might use a dht instead... because it scales better. the only thing is, i don't know how to receive notifications of an update via a dht. maybe a custom (but hopefully still scalable) solution can be worked out that allows for notifications/updates amongst siblings
    //memcached is cool and all, but i don't know what cases i would actually want to use it (referring to couchbase)... seeing as there aren't notifications to the dht... why would i ever want to read my cached results from memory?

    emit initialized();
}
#if 0
void Bank::handleBankAccountCreationRequested(const QString &username)
{
    if(!m_PerAppBankAccountsHash.value(appId).hasBankAccountFor(username))
    {
        if(m_PerAppBankAccountsHash.value(appId).addBankAccount(username))
        {
            emit bankAccountCreated(username);
            return;
        }
    }

    //todo: race condition somehow not detected by appdb (i think it's good that we double check. just because appdb is a cache does not mean we should rely on it. it answers most of the race condition requests for us but *might* miss some. it certainly should not be the finaly say of who gets to do what. we need a final check on the bank server. here it is (above: hasBankAccountFor))
    //shared race condition detection code {appdb && bank}: ---- would also imply shared "doAction" (shift variables in database for most examples). KISS... but also design well. a delicate balance
    //if the appdb were to use the same code, we could basically have duplicate code completely for the bank code and the bankhelper code
    //they both are essentially the same thing
    //they both receive bankActionRequests
    //and they both reply with bankActionResponse's
    //the bankHelper in the appdb is what holds the db of cached bank values
    //this is NOT the same database as in AppLogic (who owns what, variables of slots, the index, etc)
    //for an ad campaign, there is lastBuyPrice and lastBuyTime. from this + the user's minimumSlotPrice for the given campaign, we can calculate (and re-calculate in javascript) the current price and when it will hit the minimum. we want it to hit the minimum at exactly when the last ad slot that was purchased expires. it doubles at each slot purchase, and the rate at which it declines back to minimumSlotPrice is calculated and populated/supplied to js code on the webserver so the webserver can keep the user up to date without ever coming back to the [web]server or receiving a push/update from the [web]server

    //todo: error where addBankAccount fails (db errors?)


    emit bankAccountCreationFailed(); //i do like this generic/fallback error emitting approach
}
void Bank::handleBalanceTransferRequested(const QString &username, double amount)
{
}
#endif
void Bank::createBankAccount(const QString &username)
{
    //TODO: write to the db. this is the user's implementation... but we'd want to have exposed (maybe send them a header to #include as well as the ibank.h) our dht if used as a service like amazon aws / proprietary mode. meh. i should do open source and STILL do the business. i think your target audience is not really interested in setting up all the infrastructure and would rather use your pre-existing servers for a fee
    //that is, unless i made installing it / setting it up insanely easy via script/installer/wizard (and i could even auto-set-it-up on servers of my own... as in, rent them out seemlessly. these are a lot of ideas and decisions coming together and i think free software is the way to go because i don't want to be a greedy nigger like aws but i still appreciate the business need to just define an interface and the object implementation (dht storage is abstracted to them via header) so i think i will make enough money off of hosting the service aspect of it, including payment server etc, so businesses can just focus on business)

    //perhaps even the user interface, which is only an rpc client and in this case Wt where appdb is rpc server, can be user-generated (and/or auto-generated ;-))
    //maybe we could just give them an html string and a list of $_varName_$ 's to litter throughout it so we will string replace them. i'm not sure if wt wants html.. but there's probably a way to use raw html as the content of a WContainerWidget

    emit d("create bank account for user: " + username + " requested");

    emit bankAccountCreated(username);
}
#if 0
    Bank::CreateBankAccount(QString username)
    {
      BankAccountObject object; //stack. we don't give a shit about it after it has been streamed to QDataStream
      object.username = username;
      object.pendingBalance = 0.0;
      object.confirmedBalance = 0.0;
      object.thereWasSomeEnumHere = I_FORGET_WHAT;
      //todo: object.publicDecryptionCertificate, object.userIsSecureAsFuck, which maybe isn't necessary if we just check for a default value of publicDecryptionCertificate

      QDataStream stream << object;
      key=sha512(username + someBankOnlyKnownSalt);

      dht.put(key, object.toBase64());

      //TODO: check response: if(!dht.put.... how long does it take for it to put something. is it async? more importantly, will it tell me if a key/value pair already exists for a given key? how can i do an atomic read and if not exist, write. if exist, fail and notify of failure. that is seriously one of the hardest DHT questions ever. but it has an answer. it probably isn't _THAT_ hard, it just sounds hard. ima go take a shit after i re-read it and see if i can come up with anything. nope nothing my brain got stuck in trying to figure out routing again. i'm pretty sure THAT's the hardest problem. there are lot of simple solutions but the best solution will [probably] be quite complex

      //^^^^that dht usage actually kind of sucks. the gfs style is superior
      //also dunno why this is in here my interface when it should probably be in the impl (but obviously still commented out
    }
#endif
