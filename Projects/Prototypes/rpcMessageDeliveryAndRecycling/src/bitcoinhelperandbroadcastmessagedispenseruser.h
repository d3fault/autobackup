#ifndef BITCOINHELPERANDBROADCASTMESSAGEDISPENSERUSER_H
#define BITCOINHELPERANDBROADCASTMESSAGEDISPENSERUSER_H

#include <QObject>
#include <QThread>
#include <QHash>
#include <QTimer>

class BitcoinHelperAndBroadcastMessageDispenserUser : public QObject
{
    Q_OBJECT
public:
    explicit BitcoinHelperAndBroadcastMessageDispenserUser(QHash<BroadcastEnum, BroadcastDispenser*> *broadcastDispensers);

    void moveBroadcastDispensersToThread(QThread *thread);
private:
    BroadcastDispenser *m_PendingBalanceAddedDetectedMessageDispenser;

    QTimer *m_DebugTimer;
signals:

public slots:
    void startDebugTimer();
private slots:
    void handleDebugTimerTimeout();
};

#endif // BITCOINHELPERANDBROADCASTMESSAGEDISPENSERUSER_H
