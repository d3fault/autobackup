#include "bytearraymessagesizepeekerforiodevice.h"

ByteArrayMessageSizePeekerForIODevice::ByteArrayMessageSizePeekerForIODevice(QIODevice *ioDeviceToPeek)
{
    m_IODeviceToPeek = ioDeviceToPeek;
}
bool ByteArrayMessageSizePeekerForIODevice::enoughBytesAreAvailableToReadTheByteArrayMessage()
{
    if(m_IODeviceToPeek->bytesAvailable() < sizeof(quint32))
    {
        return false;
    }

    //m_PeekedByteArraySizeRaw.clear();

    //this probably gives me a _NEW_ QByteArray lmfao, so maybe i should use the other overload that lets me pass in a buffer?
    //m_PeekedByteArraySizeRaw = ioDeviceToPeekByteArrayMessageSizeOn->peek(sizeof(quint32));
    //qint64 actualPeekedSize = m_IODeviceToPeek->peek(m_PeekedByteArraySizeRaw.data(), sizeof(quint32));
    //m_PeekedByteArraySizeRaw.data()[sizeof(quint32)+1] = '\0';
    qint64 actualPeekedSize = m_IODeviceToPeek->peek(m_RawSizePeekCharArray, sizeof(quint32));
    m_PeekedByteArraySizeRaw.setRawData(m_RawSizePeekCharArray, sizeof(quint32));

    quint32 peekedByteArraySize;
    //qint64 bytesPeeked = newClient->peek((char*)&peekedByteArraySize /* i just threw up, but i _THINK_ this will work? */, sizeof(quint32)); //TODOreq: this might not work because we're not longer using QDataStream's encoding/decoding??? little endian etc etc, who the fuck knows. i can't remember if i wrote this or only thought it, but i could read the raw bytes (by peeking) into a byte array and then qdatastream them out of it, lmfao hack but sounds a little safer i suppose...
    //if(bytesPeeked != sizeof(quint32))
    if(actualPeekedSize != sizeof(quint32))
    {
        return false; //needs moar bytes (we should never get here beause we check bytesAvailable above)
    }
    QDataStream peekDecoderStream(&m_PeekedByteArraySizeRaw, QIODevice::ReadOnly); //TODOreq: recycle this in production use too. can't, no setByteArray function in QDataStream :-/
    peekDecoderStream >> peekedByteArraySize; //i guess re-use that quint32 also... but it's a cheap stack alloc vs. a heap lookup SO MAYBE NOT (but the QByteArray and QDataStream are heap no matter what... so.... (premature ejaculation much?))

    if(peekedByteArraySize == 0xFFFFFFFF || peekedByteArraySize == 0x0) //we still want to read in the message if it's null or empty. we 'know' we have enough bytes if it's either of these two values (null or zero). TODOreq: make sure that a zero sized array still streams a zero. it very likely does, but the doc is slightly ambiguious on the matter (whereas null arrays it explicitly says that it'll be fffetc)
    {
        return true;
    }

    if((m_IODeviceToPeek->bytesAvailable()-sizeof(quint32)) < peekedByteArraySize) //we check to see if there's enough bytesAvailable that the byteArray requires, but we don't count the quint32 'byte array size' that we peeked in that calculation
    {
        return false; //needs moar bytes!
    }

    //normal case where enough bytes are available :-D
    return true;
}
