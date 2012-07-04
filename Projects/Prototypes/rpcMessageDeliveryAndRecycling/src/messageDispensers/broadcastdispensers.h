#ifndef BROADCASTDISPENSERS_H
#define BROADCASTDISPENSERS_H

#include <QObject>

class IRpcClientsHelper;
class PendingBalanceAddedDetectedMessageDispenser;

class BroadcastDispensers : public QObject
{
    Q_OBJECT
public:
    explicit BroadcastDispensers(IRpcClientsHelper *destinationObject);
    PendingBalanceAddedDetectedMessageDispenser *pendingBalanceAddedDetectedMessageDispenser();
private:
    PendingBalanceAddedDetectedMessageDispenser *m_PendingBalanceAddedDetectedMessageDispenser;
};

#endif // BROADCASTDISPENSERS_H
