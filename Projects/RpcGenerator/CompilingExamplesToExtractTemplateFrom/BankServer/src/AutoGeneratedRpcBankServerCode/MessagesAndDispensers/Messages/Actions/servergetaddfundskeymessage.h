#ifndef SERVERGETADDFUNDSKEYMESSAGE_H
#define SERVERGETADDFUNDSKEYMESSAGE_H

#include "servergetaddfundskeymessageerrors.h"
#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessagebase.h"

class ServerGetAddFundsKeyMessage : public ServerGetAddFundsKeyMessageErrors, public GetAddFundsKeyMessageBase
{
    Q_OBJECT
public:
    ServerGetAddFundsKeyMessage(QObject *owner);

    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);
};

#endif // SERVERGETADDFUNDSKEYMESSAGE_H
