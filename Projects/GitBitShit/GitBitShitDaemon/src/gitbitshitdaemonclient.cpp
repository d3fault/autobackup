#include "gitbitshitdaemonclient.h"

#include <QIODevice>
#include <QDataStream>
#include <QBuffer>

GitBitShitDaemonClient::GitBitShitDaemonClient(QIODevice *client, QObject *parent)
    : QObject(parent)
    , m_Client(client)
{
    connect(client, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
}
void GitBitShitDaemonClient::handleReadyRead()
{
    qint64 bytesAvailable = m_Client->bytesAvailable();
    if(bytesAvailable < sizeof(quint32))
        return;
    qint64 payloadBytesAvailable = bytesAvailable - sizeof(quint32);
    quint32 messageLength = 0;
    QByteArray peekedMessageSizeByteArray = m_Client->peek(sizeof(quint32));
    if(peekedMessageSizeByteArray.size() != sizeof(quint32))
        return;
    QBuffer messageLengthReaderBuffer(&peekedMessageSizeByteArray);
    if(!messageLengthReaderBuffer.open(QIODevice::ReadOnly))
        return;
    QDataStream messageLengthDataStream(&messageLengthReaderBuffer);
    messageLengthDataStream >> messageLength;
    if(messageLength > 0 && payloadBytesAvailable >= messageLength)
    {
        QString theMessage;
        QDataStream theMessageDataStream(m_Client);
        theMessageDataStream >> theMessage;
        emit messageReceived(theMessage);
    }
#if 0
    m_Client->peek(static_cast<char*>(&messageLength), sizeof(quint32));
    if(messageLength > 0 && payloadBytesAvailable >= messageLength)
    {
        QString theMessage;
        QDataStream theMessageDataStream(m_Client);
        theMessageDataStream >> theMessage;
        emit messageReceived(theMessage);
    }
#endif
}
