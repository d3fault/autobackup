#ifndef BITCOINHELPER_H
#define BITCOINHELPER_H

#include <QObject>

class BitcoinHelper : public QObject
{
    Q_OBJECT
public:
    explicit BitcoinHelper(QObject *parent = 0);
    QString getNewKeyToReceivePaymentsAt();
signals:

public slots:

};

#endif // BITCOINHELPER_H
