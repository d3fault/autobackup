#ifndef DHT_H
#define DHT_H

#include <QObject>

class Dht : public QObject
{
    Q_OBJECT
public:
    explicit Dht(QList<QString> dhtSeedIpAddresses, QObject *parent = 0);
public slots:
    QByteArray downloadBlob(const QString &sha1ofA7zThatIsKnownToBeOnTheP2pNetwork);
};

#endif // DHT_H
