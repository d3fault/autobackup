#ifndef SAVEFILEORSTDOUT_H
#define SAVEFILEORSTDOUT_H

#include <QIODevice>

class SaveFileOrStdOut : public QIODevice
{
    Q_OBJECT
public:
    explicit SaveFileOrStdOut(const QString &fileName_OrEmptyStringForStdOut, QObject *parent = 0);
    virtual ~SaveFileOrStdOut();
    bool open(OpenMode mode);
    void setDirectWriteFallback(bool enabled);
    bool directWriteFallback() const;
    bool commitIfSaveFile();

    virtual bool isSequential() const;
    virtual qint64 pos() const;
    virtual qint64 size() const;
    virtual bool seek(qint64 pos);
    virtual bool atEnd() const;
    virtual bool reset();
    virtual qint64 bytesAvailable() const;
    virtual qint64 bytesToWrite() const;
    virtual bool canReadLine() const;
    virtual bool waitForReadyRead(int msecs);
    virtual bool waitForBytesWritten(int msecs);
protected:
    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);
private:
    QIODevice *m_IODeviceProxyingFor;
    bool m_IsStdOut;
    bool m_HaveCommitted;

    void close();
};

#endif // SAVEFILEORSTDOUT_H
