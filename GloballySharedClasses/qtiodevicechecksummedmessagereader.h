#ifndef QTIODEVICECHECKSUMMEDMESSAGEREADER_H
#define QTIODEVICECHECKSUMMEDMESSAGEREADER_H

#include <QObject>

#include "qtiodevicechecksummedmessageheaderreader.h"

class QIODevice;

class QtIoDeviceChecksummedMessageReader : public QObject
{
    Q_OBJECT
public:
    static QByteArray checksum(const QByteArray &input);
    explicit QtIoDeviceChecksummedMessageReader(QIODevice *ioDeviceToReadChecksummedMessagesFrom, QObject *parent = 0);
private:
    QIODevice *m_IoDeviceToReadChecksummedMessagesFrom;
private slots:
    void handleReadyRead();
private:
    bool messageChecksumIsValid();
    bool tryReadMessageActual(QByteArray *out_Message);
public:
    bool tryReadMessage(QByteArray *out_Message);
private:
    QByteArray m_MessageBuffer;
    QtIoDeviceChecksummedMessageHeaderReader m_MessageHeaderReader;
    int m_MessageSize;
    bool m_HaveReadMessageHeader;
    int m_NumBytesOfMessageLeft;
    QByteArray m_CalculatedMessageChecksum;

signals:
    void checksummedMessageRead(const QByteArray &message);
};

#endif // QTIODEVICECHECKSUMMEDMESSAGEREADER_H
