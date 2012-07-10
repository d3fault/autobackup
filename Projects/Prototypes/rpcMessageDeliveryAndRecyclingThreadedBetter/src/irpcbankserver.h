#ifndef IRPCBANKSERVER_H
#define IRPCBANKSERVER_H

#include <QObject>

#include "rpcbankserverclientshelper.h"

#include "messageDispensers/broadcastdispensers.h"

class IRpcBankServer : public QObject
{
    Q_OBJECT
public:
    explicit IRpcBankServer(QObject *mandatoryParent = 0);
    virtual void takeOwnershipOfAllBroadcastDispensers(RpcBankServerClientsHelper *rpcBankServerClientsHelper)=0;
    virtual void moveBackendBusinessObjectsToTheirOwnThreadsAndStartThem()=0;
public slots:
    virtual void init()=0;
    virtual void start()=0;
    virtual void stop()=0;

    virtual void createBankAccount(CreateBankAccountMessage *createBankAccountMessage)=0;
signals:
    void initialized();
    void started();
    void stopped(); //gui could listen for this before closing, but it isn't used in the daisy chaining, whereas initialized and started are (we reverse the order on shutdown/stop)

    //we could also do init as base method that calls a bool privateInit(); pure virtual (also for start/stop), returns false on fail and then we don't emit the correlating signal..
    //^^while that sounds nice, i think it's bad OO principles. why intercept return value like that and emit signals? sloppy/hacky though currently hard to visualize why. i'm sure a graph could explain it better. my instincts just say no. but why am i wasting time thinking about this. fuck it
};

#endif // IRPCBANKSERVER_H
