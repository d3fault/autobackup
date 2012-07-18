#ifndef BITCOINHELPER_H
#define BITCOINHELPER_H

#include <QObject>

class BitcoinHelper : public QObject
{
    Q_OBJECT
public:
    explicit BitcoinHelper();
    void takeOwnershipOfApplicableBroadcastDispensers(BroadcastDispensers *broadcastDispensers);
};

#endif // BITCOINHELPER_H
