#include "rpcbankserverbroadcastdispensers.h"

RpcBankServerBroadcastDispensers::RpcBankServerBroadcastDispensers(IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptAllDeliveries *destination)
    : m_Destination(destination), m_PendingBalanceDetectedMessageDispenser(0), m_ConfirmedBalanceDetectedMessageDispenser(0)
{ }
ServerPendingBalanceDetectedMessageDispenser *RpcBankServerBroadcastDispensers::takeOwnershipOfPendingBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner)
{
    QMutexLocker scopedLock(&m_CreateAndCheckCreatedMutex);
    Q_UNUSED(scopedLock)

    m_PendingBalanceDetectedMessageDispenser = new ServerPendingBalanceDetectedMessageDispenser(static_cast<IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptAllDeliveries*>(m_Destination), owner);
    return m_PendingBalanceDetectedMessageDispenser;
}
ServerConfirmedBalanceDetectedMessageDispenser *RpcBankServerBroadcastDispensers::takeOwnershipOfConfirmedBalanceDetectedMessageDispenserRiggedForDelivery(QObject *owner)
{
    QMutexLocker scopedLock(&m_CreateAndCheckCreatedMutex);
    Q_UNUSED(scopedLock)

    m_ConfirmedBalanceDetectedMessageDispenser = new ServerConfirmedBalanceDetectedMessageDispenser(static_cast<IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptAllDeliveries*>(m_Destination), owner);
    return m_ConfirmedBalanceDetectedMessageDispenser;
}
bool RpcBankServerBroadcastDispensers::everyDispenserIsCreated()
{
    QMutexLocker scopedLock(&m_CreateAndCheckCreatedMutex);
    Q_UNUSED(scopedLock)

    if(!m_PendingBalanceDetectedMessageDispenser)
        return false;
    if(!m_ConfirmedBalanceDetectedMessageDispenser)
        return false;

    return true;
}
