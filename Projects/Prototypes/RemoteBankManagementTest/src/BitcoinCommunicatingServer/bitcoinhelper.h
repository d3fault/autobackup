#ifndef BITCOINHELPER_H
#define BITCOINHELPER_H

#include <QObject>
#include <QProcess>

#define PATH_TO_BITCOIND "/home/d3fault/test/btc/bitcoin-0.5.2-linux/bin/32/"
#define BITCOIND_PROCESS "bitcoind"
#define BITCOIN_DATA_DIR "-datadir=/home/d3fault/test/btc/testnet-box/2"

class BitcoinHelper : public QObject
{
    Q_OBJECT
public:
    explicit BitcoinHelper(QObject *parent = 0);
    QString getNewKeyToReceivePaymentsAt();
private:
    QProcess *m_BitcoinD;
    QString bitcoind(QString apiCmd);
signals:

private slots:
    void handleProcessError(QProcess::ProcessError processError);
};

#endif // BITCOINHELPER_H
