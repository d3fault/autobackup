#ifndef IRECYCLEABLEDISPENSER_H
#define IRECYCLEABLEDISPENSER_H

#include <QObject>

#include "../Messages/irecycleableandstreamable.h"

//TODOreq: add string "RpcBankServer" to IMessageDispenser and to IMessage... so we can use multiple rpc client/server generated code instances side by side
class IRecycleableDispenser : public QObject
{
    Q_OBJECT
public:
    explicit IRecycleableDispenser(QObject *destination, QObject *owner);
private:
    QList<IRecycleableAndStreamable*> m_RecycleList;
protected:
    QObject *m_Destination; //TODOoptimization: broadcasts on client don't need destination, so maybe make a IMessageDispenser : IRecycleableDispenser ... where IMessageDispenser just holds the destination object and that's it (ClientBroadcastMessageDispensers skip the IMessageDispenser (don't implement them), the rest don't). it's all i need from having a pretty damn close to perfect design... HOWEVER, it doesn't matter too much because the user is UNABLE to call .deliver() in the client business anyways... so who gives a shit. all it's really doing is wasting one QObject pointer per broadcast message
    IRecycleableAndStreamable *privateGetNewOrRecycled();
    virtual IRecycleableAndStreamable *newOfTypeAndConnectToDestinationObjectIfApplicable()=0;
public slots:
    void handleMessageReportingItselfDone();
};

#endif // IRECYCLEABLEDISPENSER_H
