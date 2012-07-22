#ifndef IRPCBANKSERVERCLIENTBUSINESS_H
#define IRPCBANKSERVERCLIENTBUSINESS_H

#define RPC_BANK_SERVER_CLIENT_CODE

#include <QObject>


#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/rpcbankserveractiondispensers.h"
#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/rpcbankserverbroadcastdispensers.h"

class IRpcBankServerClientBusiness : public QObject
{
    Q_OBJECT
public:
    //TODOoptimization: could perform open heart surgery and merge some interfaces between IRpcBankServerBusiness and IRpcBankServerClientBusiness into some IRpcBusiness. just the following two pure virtuals mostly (maybe others though)
    virtual void instructBackendObjectsToClaimRelevantDispensers()=0;
    virtual void moveBackendBusinessObjectsToTheirOwnThreadsAndStartTheThreads()=0;

    virtual void connectRpcBankServerSignalsToBankClientImplSlots(IEmitRpcBankServerSignalsWithMessageAsParam *signalEmitter)=0;

    void setActionDispensers(RpcBankServerActionDispensers *actionDispensers);
protected:
    RpcBankServerActionDispensers *m_ActionDispensers;
public slots:
    virtual void init()=0;
    virtual void start()=0;
    virtual void stop()=0;
signals:
    void d(const QString &);
    void initialized();
    void started();
    void stopped();
};

#endif // IRPCBANKSERVERCLIENTBUSINESS_H
