#ifndef CONFIRMEDBALANCEDETECTEDMESSAGE_H
#define CONFIRMEDBALANCEDETECTEDMESSAGE_H

#include "../imessage.h"

#define CONFIRMED_BALANCE_DETECTED_MESSAGE_PARSE_BROADCAST(qds,strop) (qds strop Username; qds strop ConfirmedBalance;)

class ConfirmedBalanceDetectedMessage : public IMessage
{
public:
    ConfirmedBalanceDetectedMessage(QObject *owner);

    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);

    QString Username;
    double ConfirmedBalance;
};

#endif // CONFIRMEDBALANCEDETECTEDMESSAGE_H
