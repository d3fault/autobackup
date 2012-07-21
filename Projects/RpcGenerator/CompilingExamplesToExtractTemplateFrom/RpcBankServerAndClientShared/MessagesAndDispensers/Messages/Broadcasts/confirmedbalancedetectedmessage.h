#ifndef CONFIRMEDBALANCEDETECTEDMESSAGE_H
#define CONFIRMEDBALANCEDETECTEDMESSAGE_H

#include "../imessage.h"

//it is still beneficial for broadcasts to use this define so there is still only 1 official definition (not that it matters since we're generated code)
#define CONFIRMED_BALANCE_DETECTED_MESSAGE_PARSE_BROADCAST(qds,strop) qds strop Username; qds strop ConfirmedBalance;

class ConfirmedBalanceDetectedMessage : public IMessage
{
    Q_OBJECT
public:
    ConfirmedBalanceDetectedMessage(QObject *owner);

    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);

    QString Username;
    double ConfirmedBalance;
};

#endif // CONFIRMEDBALANCEDETECTEDMESSAGE_H
