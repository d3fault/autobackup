#include "qtiodevicechecksummedmessagereader.h"

#include <QIODevice>
#include <QCryptographicHash>

QByteArray QtIoDeviceChecksummedMessageReader::checksum(const QByteArray &input)
{
    return QCryptographicHash::hash(input, QCryptographicHash::Md5).toHex(); //MD5 sucks (but will suffice for now). provide a way for the user to provide their own checksum function
}
QtIoDeviceChecksummedMessageReader::QtIoDeviceChecksummedMessageReader(QIODevice *ioDeviceToReadChecksummedMessagesFrom, QObject *parent)
    : QObject(parent)
    , m_IoDeviceToReadChecksummedMessagesFrom(ioDeviceToReadChecksummedMessagesFrom)
    , m_MessageHeaderReader(ioDeviceToReadChecksummedMessagesFrom)
    , m_HaveReadMessageHeader(false)
{
    connect(ioDeviceToReadChecksummedMessagesFrom, &QIODevice::readyRead, this, &QtIoDeviceChecksummedMessageReader::handleReadyRead);
}
void QtIoDeviceChecksummedMessageReader::handleReadyRead()
{
    QByteArray inputCommandBuffer;
    while(m_IoDeviceToReadChecksummedMessagesFrom->bytesAvailable())
    {
        if(tryReadMessage(&inputCommandBuffer))
            emit checksummedMessageRead(inputCommandBuffer);
    }
}
bool QtIoDeviceChecksummedMessageReader::messageChecksumIsValid()
{
    m_CalculatedMessageChecksum = checksum(m_MessageBuffer);
    if(m_CalculatedMessageChecksum == m_MessageHeaderReader.parsedMessageChecksum())
    {
        return true;
    }
    else
    {
        qFatal("m_CalculatedMessageChecksum != m_MessageHeaderReader->parsedMessageChecksum()"); //TODOreq: remove this call to fatal when "requesting message is re-sent" is implemented
    }
    return false;
}
bool QtIoDeviceChecksummedMessageReader::tryReadMessageActual(QByteArray *out_Message)
{
    int numBytesToRead = qMin(m_NumBytesOfMessageLeft, static_cast<int>(m_IoDeviceToReadChecksummedMessagesFrom->bytesAvailable()));
    if(numBytesToRead < 1)
        return false;
    for(int i = 0; i < numBytesToRead; ++i)
    {
        char inChar;
        if(!m_IoDeviceToReadChecksummedMessagesFrom->getChar(&inChar))
        {
            m_HaveReadMessageHeader = false;
            return false;
        }
        m_MessageBuffer += inChar; //TODOoptimization: read chunks. implement this LATER
    }
    m_NumBytesOfMessageLeft -= numBytesToRead;
    if(m_NumBytesOfMessageLeft < 1)
    {
        //woot read in all of a message
        //now verify it's checksum
        m_HaveReadMessageHeader = false; //so next time we call tryReadMessage we'll be back to looking for the header
        if(messageChecksumIsValid())
        {
            *out_Message = m_MessageBuffer;
            return true;
        }
        return false;
    }
    return false;
}
bool QtIoDeviceChecksummedMessageReader::tryReadMessage(QByteArray *out_Message)
{
    if(!m_HaveReadMessageHeader)
    {
        if(!m_MessageHeaderReader.tryReadMessageHeader(&m_MessageSize))
        {
            return false;
        }
        else
        {
#if 0 //wasn't this just a limitation of the arduino? well yes and no but I mean it's a good idea to have the MaxMessageSize be passed to the constructor. eh fuck it on PC world you just assume you don't run out of memory ;-). I mean for now, implementing that safety is for a rainy day
            if(m_MessageSize > MAX_MESSAGE_SIZE)
            {
                qCritical() << "Message Size: " << m_MessageSize;
                qFatal("m_MessageSize > MAX_MESSAGE_SIZE");
                return false;
            }
#endif
            m_HaveReadMessageHeader = true;
            m_NumBytesOfMessageLeft = m_MessageSize;
            m_MessageBuffer.clear();
        }
    }

    //if we get here we've read the message header. either just now or on a previous call to tryReadMessage. it also means m_MessageSize is valid
    return tryReadMessageActual(out_Message);
}
