#ifndef IMESSAGEDISPENSER_H
#define IMESSAGEDISPENSER_H

#include <QObject>

#include "../Messages/imessage.h"

//TODOreq: add "RpcBankServer" to IMessageDispenser and to IMessage... so we can use multiple rpc client/server generated code instances side by side
class IMessageDispenser : public QObject
{
    Q_OBJECT
public:
    explicit IMessageDispenser(QObject *destination, QObject *owner);
private:
    QList<IMessage*> m_RecycleList;
    QObject *m_Destination;
protected:
    IMessage *privateGetNewOrRecycled();
    virtual IMessage *getNewOfTypeAndConnectToDestinationObject()=0;
public slots:
    void handleMessageReportingItselfDone();

#endif // IMESSAGEDISPENSER_H
