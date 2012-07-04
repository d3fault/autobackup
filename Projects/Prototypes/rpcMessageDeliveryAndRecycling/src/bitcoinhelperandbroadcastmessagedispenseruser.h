#ifndef BITCOINHELPERANDBROADCASTMESSAGEDISPENSERUSER_H
#define BITCOINHELPERANDBROADCASTMESSAGEDISPENSERUSER_H

#include <QObject>
#include <QThread>
#include <QHash>
#include <QTimer>

#include "messageDispensers/broadcastdispensers.h"

class BitcoinHelperAndBroadcastMessageDispenserUser : public QObject
{
    Q_OBJECT
public:
    explicit BitcoinHelperAndBroadcastMessageDispenserUser(BroadcastDispensers *broadcastDispensers);

    void moveBroadcastDispensersToThread(QThread *thread);
private:
    PendingBalanceAddedDetectedMessageDispenser *m_PendingBalanceAddedDetectedMessageDispenser;

    QTimer *m_DebugTimer;
signals:

public slots:
    void startDebugTimer();
private slots:
    void handleDebugTimerTimeout();
};

#endif // BITCOINHELPERANDBROADCASTMESSAGEDISPENSERUSER_H
