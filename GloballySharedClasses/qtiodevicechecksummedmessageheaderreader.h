#ifndef QTIODEVICECHECKSUMMEDMESSAGEHEADERREADER_H
#define QTIODEVICECHECKSUMMEDMESSAGEHEADERREADER_H

#include <QByteArray>

class QIODevice;

class QtIoDeviceChecksummedMessageHeaderReader
{
public:
    explicit QtIoDeviceChecksummedMessageHeaderReader(QIODevice *ioDevice);
private:
    QIODevice *m_IoDevice;
private:
    enum MessageHeaderReaderState
    {
          LookingForSync = 0
        , ReadingChecksumOfSizeOfData = 1
        , ReadingSizeOfData = 2
        , ReadingChecksumOfData = 3
    };
    MessageHeaderReaderState m_State;

    bool tryReadUntilComma();
public:
    bool tryReadMessageHeader(int *out_MessageSize);
    const QByteArray &parsedMessageChecksum() const;
private:
    const int SizeOfLargestHeaderComponent;

    QByteArray m_MessageHeaderBuffer;

    QByteArray m_ParsedChecksumOfSizeOfData;
    int m_ParsedSizeOfData;
    QByteArray m_ParsedSizeOfDataAsString;
    QByteArray m_CalculatedChecksumOfSizeOfData;

    QByteArray m_ParsedMessageChecksum;
#if 1 //TODOreq: merge this copypasta from arduino side. qt-ize it simultaneously <3. all I'm doing is moving where this #if 1 is located by pressing shift+ctrl+down
#endif
};
#endif // QTIODEVICECHECKSUMMEDMESSAGEHEADERREADER_H
