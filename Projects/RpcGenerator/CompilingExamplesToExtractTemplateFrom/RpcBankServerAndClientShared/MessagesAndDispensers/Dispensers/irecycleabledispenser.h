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
    QObject *m_Destination;
    IRecycleableAndStreamable *privateGetNewOrRecycled();
    virtual IRecycleableAndStreamable *newOfTypeAndConnectToDestinationObjectIfApplicable()=0;
public slots:
    void handleMessageReportingItselfDone();
};

#endif // IRECYCLEABLEDISPENSER_H
