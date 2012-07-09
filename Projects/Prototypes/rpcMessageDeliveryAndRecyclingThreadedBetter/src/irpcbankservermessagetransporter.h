#ifndef IRPCBANKSERVERMESSAGETRANSPORTER_H
#define IRPCBANKSERVERMESSAGETRANSPORTER_H

#include <QObject>

#include "messages/actions/createbankaccountmessage.h"

class IRpcBankServerMessageTransporter : public QObject
{
    Q_OBJECT
public:
    explicit IRpcBankServerMessageTransporter(QObject *mandatoryParent = 0);
signals:
    void createBankAccount(CreateBankAccountMessage *createBankAccountMessage);
public slots:
    void createBankAccountCompleted();
    void createBankAccountFailedUsernameAlreadyExists();
    void createBankAccountFailedPersistError();

    void pendingBalanceDetected();

protected:
    //not sure if i should have a pure virtual 'transportToClient'... or if all of my public slots should be pure virtual...

    //i mean i gotta factor in how the createBankAccount signal will be used

    //i think the transporter impl should keep track of who the shit came from... and who to send it back to. it could be either an rpc client id... or a socket pointer (except if it's socket pointer, connection loss makes it harder to figure out the new pointer)

    //the equivalent/opposite of 'transportToClient' would be 'messageReceivedFromClient' ... which i already have coded. it is very specific to the impl... so it should probably be an impl. should it be a pure virtual or anything? idfk

    //the only thing is that making this interface be the one that keeps track of who to send back to etc is that i don't have to do it a different way (or even the same way... just... again) for other impls

    //and i need to make sure my 'send back to' method is broadcast compliant (not sending BACK... but sending to a specific)
};

#endif // IRPCBANKSERVERMESSAGETRANSPORTER_H
