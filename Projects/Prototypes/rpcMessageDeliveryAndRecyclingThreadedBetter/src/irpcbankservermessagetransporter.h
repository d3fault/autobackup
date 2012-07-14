#ifndef IRPCBANKSERVERMESSAGETRANSPORTER_H
#define IRPCBANKSERVERMESSAGETRANSPORTER_H

#include <QObject>
#include <QThread> //debug
#include <QDateTime> //debug

#include "messages/actions/createbankaccountmessage.h"

class RpcBankServerClientsHelper;
class CreateBankAccountMessageDispenser;

class IRpcBankServerMessageTransporter : public QObject
{
    Q_OBJECT
public:
    explicit IRpcBankServerMessageTransporter();
    void takeOwnershipOfAllActionDispensers(RpcBankServerClientsHelper *rpcBankServerClientsHelper);
signals:
    void d(const QString &);
    void initialized();
    void started();
    void stopped();
    void createBankAccount(CreateBankAccountMessage *createBankAccountMessage);
public slots:
    void init();
    void start();
    void stop();

    void createBankAccountCompleted();
    void createBankAccountFailedUsernameAlreadyExists();
    void createBankAccountFailedPersistError();

    void pendingBalanceDetected();

    void simulateCreateBankAccount();
protected:
    CreateBankAccountMessageDispenser *m_CreateBankAccountMessageDispenser;
    //not sure if i should have a pure virtual 'transportToClient'... or if all of my public slots should be pure virtual...
    //^^^^TODO: this is actually a relatively large design decision and i don't know off the top of my head what the right solution is
    //if i don't make them pure virtual, i can have this ITransporter be the one keeping track of who to send back to etc
    //i really don't know what to do for that. especially since broadcasts don't even have that data as handily available
    //this is a good commit point

    //LEFT OFF^^^
    //^^copy/pasted this folder/state because i do think it's worth prototyping. about to add debug code to and test THIS prototype (just the message threading), but may need to copy/paste the outcome (should i change it) again before doing the client request matching prototype. the client request matching also [seems to depend heavily] on the usage of rpc server/client shit

    //old comments:

    //i mean i gotta factor in how the createBankAccount signal will be used

    //i think the transporter impl should keep track of who the shit came from... and who to send it back to. it could be either an rpc client id... or a socket pointer (except if it's socket pointer, connection loss makes it harder to figure out the new pointer)

    //the equivalent/opposite of 'transportToClient' would be 'messageReceivedFromClient' ... which i already have coded. it is very specific to the impl... so it should probably be an impl. should it be a pure virtual or anything? idfk

    //the only thing is that making this interface be the one that keeps track of who to send back to etc is that i don't have to do it a different way (or even the same way... just... again) for other impls

    //and i need to make sure my 'send back to' method is broadcast compliant (not sending BACK... but sending to a specific)
};

#endif // IRPCBANKSERVERMESSAGETRANSPORTER_H
