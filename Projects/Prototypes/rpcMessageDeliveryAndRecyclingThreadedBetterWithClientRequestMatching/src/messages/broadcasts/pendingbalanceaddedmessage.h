#ifndef PENDINGBALANCEADDEDMESSAGE_H
#define PENDINGBALANCEADDEDMESSAGE_H

#include "../imessage.h"

class PendingBalanceAddedMessage : public IMessage
{
public:
    explicit PendingBalanceAddedMessage(QObject *mandatoryParent);
    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);

    QString Username;
    double PendingBalance;
    //maybe bank sub-account username is associated with? create bank account messages need this information too at some point... however, actions know who to respond to because the request is pending... but broadcasts have to figure it out some other (more persistent) way..
};

#endif // PENDINGBALANCEADDEDMESSAGE_H
