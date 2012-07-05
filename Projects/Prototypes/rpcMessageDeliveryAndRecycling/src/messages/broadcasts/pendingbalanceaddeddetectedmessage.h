#ifndef PENDINGBALANCEADDEDDETECTEDMESSAGE_H
#define PENDINGBALANCEADDEDDETECTEDMESSAGE_H

#include "../imessage.h"

class PendingBalanceAddedDetectedMessage : public IMessage
{
    Q_OBJECT
public:
    void myDeliver();
signals:
    void pendingBalanceAddedDetected();
    //void pendingBalancedAddedDetected(PendingBalanceAddedDetectedMessage* pendingBalanceAddedDetectedMessage); //TODOreq: maybe we can just do a static_cast of the sender() in the receiving slot? it would save us the overhead of sending another pointer. stupid to send ourselves in a signal we emit... sender() is here for that reason (thought it does technically break object oriented programming rules).
    //i think that's why i made a deliverSignal() concept. it didn't account for failed reasons yet... but the delivery of messages is always going to be the same. we can always just connect the deliverSignal to a specified slot (handlePending...) and do sender() to get the message...
    //but the fact that 'deliver' doesn't (yet) account for failed reasons (multiple, specific (though i guess that means that IDeliver just doesn't have to know about them... we specify them, just as we specify the 'deliver' and call it pendingBalancedAddedDetected just now)) is reason enough to not use it. it really doesn't matter at all... i'm just obsessing over specifics
protected:
    //void privateDeliver();
};

#endif // PENDINGBALANCEADDEDDETECTEDMESSAGE_H
