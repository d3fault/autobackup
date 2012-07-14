#ifndef BITCOINHELPER_H
#define BITCOINHELPER_H

#include <QObject>
#include <QTimer>
#include <QThread> //debug

#include "rpcbankserverclientshelper.h"
#include "messageDispensers/broadcasts/pendingbalanceaddedmessagedispenser.h"

class BitcoinHelper : public QObject
{
    Q_OBJECT
public:
    explicit BitcoinHelper(); //we choose not to have a parent so we don't ever become someone's child and follow them on moveToThread

    void takeOwnershipOfApplicableBroadcastDispensers(RpcBankServerClientsHelper *rpcBankServerClientsHelper);
private:
    PendingBalanceAddedMessageDispenser *m_PendingBalanceAddedMessageDispenser;
    QTimer *m_Timer;
    double m_I;
signals:
    void d(const QString &);
    void initialized();
    void started();
    void stopped();
public slots:
    void init();
    void start();
    void stop();
private slots:
    void handleTimeout();
};

#endif // BITCOINHELPER_H
