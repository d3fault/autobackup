#ifndef PENDINGBALANCEDETECTEDMESSAGE_H
#define PENDINGBALANCEDETECTEDMESSAGE_H

#include "../imessage.h"

#define PENDING_BALANCE_DETECTED_MESSAGE_PARSE_BROADCAST(qds,strop) qds strop Username; qds strop PendingBalance;

class PendingBalanceDetectedMessage : public IMessage
{
    Q_OBJECT
public:
    PendingBalanceDetectedMessage(QObject *owner);

    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);

    QString Username;
    double PendingBalance;
};

#endif // PENDINGBALANCEDETECTEDMESSAGE_H
