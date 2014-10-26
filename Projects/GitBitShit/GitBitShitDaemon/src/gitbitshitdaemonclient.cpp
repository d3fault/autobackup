#include "gitbitshitdaemonclient.h"

#include <QLocalSocket>
#include <QDataStream>
#include <QBuffer>

GitBitShitDaemonClient::GitBitShitDaemonClient(QLocalSocket *client, QObject *parent)
    : QObject(parent)
    , m_Client(client)
{
    connect(client, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
}
void GitBitShitDaemonClient::disconnectClient()
{
    m_Client->disconnectFromServer();
}
GitBitShitDaemonClient::~GitBitShitDaemonClient()
{
    m_Client->deleteLater();
}
void GitBitShitDaemonClient::handleReadyRead()
{
    qint64 bytesAvailable = m_Client->bytesAvailable();
    if(bytesAvailable < static_cast<qint64>(sizeof(quint32)))
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
    else if(messageLength == 0)
    {
        //read the empty message, just to get it out of the way. this was probably an error
        QString emptyMessage;
        QDataStream theMessageDataStream(m_Client);
        theMessageDataStream >> emptyMessage;
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
void GitBitShitDaemonClient::handleClientDisconnected()
{
    emit clientDisconnected(this);
}
