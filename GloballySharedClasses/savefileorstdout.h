#ifndef SAVEFILEORSTDOUT_H
#define SAVEFILEORSTDOUT_H

#include <QIODevice>

class SaveFileOrStdOut : public QIODevice
{
    Q_OBJECT
public:
    SaveFileOrStdOut(const QString &fileName_OrEmptyStringForStdOut, QObject *parent = 0);
    ~SaveFileOrStdOut();
    bool open(OpenMode mode);
    void setDirectWriteFallback(bool enabled);
    bool directWriteFallback() const;
protected:
    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);
private:
    QIODevice *m_IODeviceProxyingFor;
    QString m_FileName_OrEmptyStringForStdOut;
    bool m_IsStdOut;

    void close();
};

#endif // SAVEFILEORSTDOUT_H
