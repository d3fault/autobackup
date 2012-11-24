#ifndef BYTEARRAYMESSAGESIZEPEEKERFORIODEVICE_H
#define BYTEARRAYMESSAGESIZEPEEKERFORIODEVICE_H

#include <QIODevice>
#include <QByteArray>
#include <QDataStream>
#include <QBuffer>

//TODOreq: this is NOT thread safe, since they all use the same byte array and data stream [as an optimization]. Magic _IS_ thread-safe because each connection (and therefore thread) has it's own NetworkMagic object. This is just a peek helper utility so it's a bit different in that respect

//TODOoptimization: along with making it thread safe, this can also benefit from being transformed into a macro OR SOMETHING (perhaps just pass in the IODevice to latch onto in the constructor etc). this is ridiculously similar to how i can/should optimize network magic.
//'inlining' this class and magic is a definite step forward, but basically i want to minimize QDataStream instantiations as well as QDataStream::setDevice() calls... (for both this and magic)
class ByteArrayMessageSizePeekerForIODevice
{
public:
    ByteArrayMessageSizePeekerForIODevice(QIODevice *ioDeviceToPeek);
    bool enoughBytesAreAvailableToReadTheByteArrayMessage();
private:
    char m_RawSizePeekCharArray[sizeof(quint32)];
    QIODevice *m_IODeviceToPeek;
    QByteArray m_PeekedByteArraySizeRaw;
};

#endif // BYTEARRAYMESSAGESIZEPEEKERFORIODEVICE_H
