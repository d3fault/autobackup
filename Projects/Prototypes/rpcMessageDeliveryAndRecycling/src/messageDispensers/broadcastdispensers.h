#ifndef BROADCASTDISPENSERS_H
#define BROADCASTDISPENSERS_H

#include <QObject>

#include "broadcasts/pendingbalanceaddeddetectedmessagedispenser.h"

class BroadcastDispensers : public QObject
{
    Q_OBJECT
public:
    explicit BroadcastDispensers(QObject *parent = 0)
        : QObject(parent)
    {
        m_PendingBalanceAddedDetectedMessageDispenser = new PendingBalanceAddedDetectedMessageDispenser();
    }

    PendingBalanceAddedDetectedMessageDispenser *pendingBalanceAddedDetectedMessageDispenser();
private:
    PendingBalanceAddedDetectedMessageDispenser *m_PendingBalanceAddedDetectedMessageDispenser;
};

#endif // BROADCASTDISPENSERS_H
