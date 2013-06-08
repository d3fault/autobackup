#include "abstractconnection.h"

AbstractConnection::AbstractConnection(QIODevice *ioDeviceToPeer, QObject *parent) :
    QObject(parent), m_DataStreamToPeer(ioDeviceToPeer), m_NetworkMagic(ioDeviceToPeer)
{ }
