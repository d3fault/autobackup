#include "savefileorstdout.h"

#include <QFile>
#include <QSaveFile>

//TODOportability: fukken winblows idfk how to write to stdout, maybe it works maybe it doesn't~
SaveFileOrStdOut::SaveFileOrStdOut(const QString &fileName_OrEmptyStringForStdOut, QObject *parent)
    : QIODevice(parent)
    , m_FileName_OrEmptyStringForStdOut(fileName_OrEmptyStringForStdOut)
    , m_IsStdOut(fileName_OrEmptyStringForStdOut.isEmpty())
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
}
SaveFileOrStdOut::~SaveFileOrStdOut()
{
    if(m_IODeviceProxyingFor->isOpen() && (!m_IsStdOut))
    {
        static_cast<QSaveFile*>(m_IODeviceProxyingFor)->commit();
    }
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
