#ifndef BITCOINHELPERANDBROADCASTMESSAGEDISPENSERUSER_H
#define BITCOINHELPERANDBROADCASTMESSAGEDISPENSERUSER_H

#include <QObject>
#include <QThread>
#include <QHash>
#include <QTimer>

class BroadcastDispensers;
class PendingBalanceAddedDetectedMessageDispenser;

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
    void d(const QString &);
public slots:
    void startDebugTimer();
private slots:
    void handleDebugTimerTimeout();
};

#endif // BITCOINHELPERANDBROADCASTMESSAGEDISPENSERUSER_H
