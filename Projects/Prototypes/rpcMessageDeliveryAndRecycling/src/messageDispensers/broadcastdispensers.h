#ifndef BROADCASTDISPENSERS_H
#define BROADCASTDISPENSERS_H

#include <QObject>

#include "irpcclientshelper.h"
#include "broadcasts/pendingbalanceaddeddetectedmessagedispenser.h"

class BroadcastDispensers : public QObject
{
    Q_OBJECT
public:
    explicit BroadcastDispensers(IRpcClientsHelper *destinationObject)
    {
        m_PendingBalanceAddedDetectedMessageDispenser = new PendingBalanceAddedDetectedMessageDispenser(destinationObject);
    }

    PendingBalanceAddedDetectedMessageDispenser *pendingBalanceAddedDetectedMessageDispenser();
private:
    PendingBalanceAddedDetectedMessageDispenser *m_PendingBalanceAddedDetectedMessageDispenser;
};

#endif // BROADCASTDISPENSERS_H
