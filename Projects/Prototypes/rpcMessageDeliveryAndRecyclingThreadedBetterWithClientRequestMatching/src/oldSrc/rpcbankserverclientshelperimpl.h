#ifndef RPCBANKSERVERCLIENTSHELPERIMPL_H
#define RPCBANKSERVERCLIENTSHELPERIMPL_H

#include "irpcbankserverclientshelper.h"

//the only reason i can think of off the top of my head for why to have an interface and an impl for rpc business controller (aside from the fact that it's already here (i just renamed it from something else)) is that i can have the impl be the only one that is 'aware of' (by including) the server/protocol-knower. this doesn't do... well... anything... but is sort of good design. seeing as this is all auto-generated code, it doesn't do much of anything.

//the interface takes care of setting up back end business objects (pure virtual), dispenser ownership (on said back end objects), moving the business object to it's own thread, starting it, stopping it...
//...and even connecting to it's Action/Broadcast signals (to it's own signals/slots... which are the direct opposite)

//i guess the interface can have some pure virtual like 'transmitMessageToClient(IMessage*)' ... which the impl overrides and then uses the server/protocol knower (protocol knower might be an interface too (with server (ssl/tcp in this case) as the impl))

//THIS impl just 'knows'... or... 'hasA' ssl/tcp server/protocol-knower. perhaps even in the form of an IProtocolKnower (since the transmission means may vary) <-- TODOoptimization: for now, just get server/protocol-knower WORKING. abstract IProtocolKnower out of it later (if it's even possible)

//tl;dr: this impl is not a huge benefit to us. i guess i should say the interface/impl separation isn't that huge of a benefit (you could put the blame on the interface and be just as right)

class RpcBankServerClientsHelperImpl : public IRpcBankServerClientsHelper
{
    Q_OBJECT
public:
    explicit RpcBankServerClientsHelperImpl(IRpcBankServer *business);
private:
protected:
    IRpcBankServerMessageTransporter *getNewTransporterImpl();
signals:

public slots:

};

#endif // RPCBANKSERVERCLIENTSHELPERIMPL_H
