#ifndef SAVEFILEORSTDOUT_H
#define SAVEFILEORSTDOUT_H

#include <QIODevice>

class SaveFileOrStdOut : public QIODevice
{
    Q_OBJECT
public:
    SaveFileOrStdOut(const QString &fileName_OrEmptyStringForStdOut, QObject *parent = 0);
    //SaveFileOrStdOut(const QString &fileName_OrEmptyStringForStdOut, QFlags<QIODevice::OpenModeFlag> openModeFlags = QIODevice::WriteOnly, QObject *parent = 0);
    ~SaveFileOrStdOut();
    //bool isValid() const;
    bool open(OpenMode mode);
    void setDirectWriteFallback(bool enabled);
    bool directWriteFallback() const;
protected:
    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);
private:
    QIODevice *m_IODeviceBeingProxyingFor;
    QString m_FileName;
    //bool m_IsValid;
    bool m_IsStdOut;

    void privateConstructor(const QString &fileName);
    void close();
};

#endif // SAVEFILEORSTDOUT_H
