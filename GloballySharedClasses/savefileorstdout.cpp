#include "savefileorstdout.h"

#include <QFile>
#include <QSaveFile>

//TODOportability: fukken winblows idfk how to write to stdout, maybe it works maybe it doesn't~
SaveFileOrStdOut::SaveFileOrStdOut(const QString &fileName_OrEmptyStringForStdOut, QObject *parent)
    : QIODevice(parent)
    , m_IsStdOut(fileName_OrEmptyStringForStdOut.isEmpty())
    , m_HaveCommitted(false)
{
    if(m_IsStdOut)
    {
        QFile *stdOutFile = new QFile(this);
        m_IODeviceProxyingFor = stdOutFile;
    }
    else
    {
        QSaveFile *saveFile = new QSaveFile(fileName_OrEmptyStringForStdOut, this);
        m_IODeviceProxyingFor = saveFile;
    }

    connect(m_IODeviceProxyingFor, SIGNAL(bytesWritten(qint64)), this, SIGNAL(bytesWritten(qint64)));
    connect(m_IODeviceProxyingFor, SIGNAL(aboutToClose()), this, SIGNAL(aboutToClose())); //QSaveFile::commit calls QFileDevice::close, despite us and QSaveFile marking close private :-P
}
SaveFileOrStdOut::~SaveFileOrStdOut()
{
    commitIfSaveFile(); //dgaf return value
}
bool SaveFileOrStdOut::open(QIODevice::OpenMode mode)
{
    if((mode & ReadOnly) || (mode & Append))
    {
        qWarning("SaveFileOrStdOut::open: Unsupported mode 0x%x", int(mode));
        return false;
    }

    bool ret;
    if(m_IsStdOut)
        ret = static_cast<QFile*>(m_IODeviceProxyingFor)->open(stdout, mode);
    else
        ret = m_IODeviceProxyingFor->open(mode);

    if(ret)
        QIODevice::open(mode);
    return ret;
}
void SaveFileOrStdOut::setDirectWriteFallback(bool enabled)
{
    //you can set it, but you can't query it (otherwise we'd need to store/sync a local copy of whether or not it's enabled), because we should return whatever they set, even if we're stdout!
    if(!m_IsStdOut)
        static_cast<QSaveFile*>(m_IODeviceProxyingFor)->setDirectWriteFallback(enabled);
}
bool SaveFileOrStdOut::commitIfSaveFile()
{
    if(!m_IODeviceProxyingFor->isOpen())
        return false;
    if(m_HaveCommitted)
        return false;
    m_HaveCommitted = true;

    if(m_IsStdOut)
        return true;

    return static_cast<QSaveFile*>(m_IODeviceProxyingFor)->commit();
}
bool SaveFileOrStdOut::isSequential() const
{
    return m_IODeviceProxyingFor->isSequential();
}
qint64 SaveFileOrStdOut::pos() const
{
    return m_IODeviceProxyingFor->pos();
}
qint64 SaveFileOrStdOut::size() const
{
    return m_IODeviceProxyingFor->size();
}
bool SaveFileOrStdOut::seek(qint64 pos)
{
    return m_IODeviceProxyingFor->seek(pos);
}
bool SaveFileOrStdOut::atEnd() const
{
    return m_IODeviceProxyingFor->atEnd();
}
bool SaveFileOrStdOut::reset()
{
    return m_IODeviceProxyingFor->reset();
}
qint64 SaveFileOrStdOut::bytesAvailable() const
{
    return 0;
}
qint64 SaveFileOrStdOut::bytesToWrite() const
{
    return m_IODeviceProxyingFor->bytesToWrite();
}
bool SaveFileOrStdOut::canReadLine() const
{
    return false;
}
bool SaveFileOrStdOut::waitForReadyRead(int msecs)
{
    Q_UNUSED(msecs)
    return false;
}
bool SaveFileOrStdOut::waitForBytesWritten(int msecs)
{
    return m_IODeviceProxyingFor->waitForBytesWritten(msecs);
}
qint64 SaveFileOrStdOut::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)
    qFatal("SaveFileOrStdOut::readData is called");
}
qint64 SaveFileOrStdOut::writeData(const char *data, qint64 len)
{
    return m_IODeviceProxyingFor->write(data, len);
}
void SaveFileOrStdOut::close()
{
    qFatal("SaveFileOrStdOut::close is called");
}
