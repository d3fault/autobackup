#ifndef BITCOINHELPER_H
#define BITCOINHELPER_H

#include <QObject>
#include <QProcess>

#define PATH_TO_BITCOIND "/path/to/bitcoind"

class BitcoinHelper : public QObject
{
    Q_OBJECT
public:
    explicit BitcoinHelper(QObject *parent = 0);
    QString getNewKeyToReceivePaymentsAt();
private:
    QProcess *m_BitcoinD;
signals:

private slots:
    void handleProcessError(QProcess::ProcessError processError);
};

#endif // BITCOINHELPER_H
