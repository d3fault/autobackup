#include "bytearraymessagesizepeekerforiodevice.h"

ByteArrayMessageSizePeekerForIODevice::ByteArrayMessageSizePeekerForIODevice(QIODevice *ioDeviceToPeek)
    : m_IODeviceToPeek(ioDeviceToPeek)
{
    m_PeekedByteArraySizeRaw.setRawData(m_RawSizePeekCharArray, sizeof(quint32));
    m_PeekedByteArraySizeRawBuffer.setBuffer(&m_PeekedByteArraySizeRaw);
    m_PeekedByteArraySizeRawBuffer.open(QIODevice::ReadOnly);
    m_PeekedByteArraySizeDataStream.setDevice(&m_PeekedByteArraySizeRawBuffer);

    //fucking sweet, i love Qt here/now. A 4-byte piece of memory is pointed to by a QByteArray, which is pointed to by a QBuffer (QIODevice), which is pointed to by a QDataStream. All 3 essentially point to the same 4-byte piece of memory :-D
}
bool ByteArrayMessageSizePeekerForIODevice::enoughBytesAreAvailableToReadTheByteArrayMessage()
{
    qint64 bytesAvailable = m_IODeviceToPeek->bytesAvailable();
    if(bytesAvailable < sizeof(quint32))
    {
        return false;
    }

    //m_PeekedByteArraySizeRaw.clear();

    //this probably gives me a _NEW_ QByteArray lmfao, so maybe i should use the other overload that lets me pass in a buffer?
    //m_PeekedByteArraySizeRaw = ioDeviceToPeekByteArrayMessageSizeOn->peek(sizeof(quint32));
    //qint64 actualPeekedSize = m_IODeviceToPeek->peek(m_PeekedByteArraySizeRaw.data(), sizeof(quint32));
    //m_PeekedByteArraySizeRaw.data()[sizeof(quint32)+1] = '\0';
    qint64 actualPeekedSize = m_IODeviceToPeek->peek(m_RawSizePeekCharArray, sizeof(quint32));
    m_PeekedByteArraySizeRawBuffer.seek(0);

    if(actualPeekedSize != sizeof(quint32))
    {
        return false; //needs moar bytes (we should never get here beause we check bytesAvailable above)
    }
    quint32 peekedByteArraySize;
    m_PeekedByteArraySizeDataStream >> peekedByteArraySize;

    if(peekedByteArraySize == 0xFFFFFFFF || peekedByteArraySize == 0x0) //we still want to read in the message if it's null or empty. we 'know' we have enough bytes if it's either of these two values (null or zero). TODOreq: make sure that a zero sized array still streams a zero. it very likely does, but the doc is slightly ambiguious on the matter (whereas null arrays it explicitly says that it'll be fffetc)
    {
        return true;
    }

    if(bytesAvailable < (peekedByteArraySize+sizeof(quint32))) //we check to see if there's enough bytesAvailable that the byteArray requires, but we need to also count the quint32 'byte array size' that we peeked in that calculation
    {
        return false; //needs moar bytes!
    }

    //normal case where enough bytes are available :-D
    return true;
}
