#include "dht.h"

Dht::Dht(QList<QString> dhtSeedIpAddresses, QObject *parent)
    : QObject(parent)
{ }
QByteArray Dht::downloadBlob(const QString &sha1ofA7zThatIsKnownToBeOnTheP2pNetwork)
{
    //libtorrent-rasterbar interactions go here
}
