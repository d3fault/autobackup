#include "savefileorstdout.h"

#include <QFile>
#include <QSaveFile>

SaveFileOrStdOut::SaveFileOrStdOut(const QString &fileName_OrEmptyStringForStdOut, QObject *parent)
    : QIODevice(parent)
{
    privateConstructor(fileName_OrEmptyStringForStdOut/*, QIODevice::WriteOnly*/);
}
#if 0
SaveFileOrStdOut::SaveFileOrStdOut(const QString &fileName_OrEmptyStringForStdOut, QFlags<OpenModeFlag> openModeFlags, QObject *parent)
    : QFileDevice(parent)
{
    privateConstructor(fileName_OrEmptyStringForStdOut, openModeFlags);
}
#endif
SaveFileOrStdOut::~SaveFileOrStdOut()
{
    if(m_IODeviceBeingProxyingFor->isOpen() && (!m_IsStdOut))
    {
        static_cast<QSaveFile*>(m_IODeviceBeingProxyingFor)->commit();
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
        ret = static_cast<QFile*>(m_IODeviceBeingProxyingFor)->open(stdout, mode);
    else
        ret = m_IODeviceBeingProxyingFor->open(mode);

    if(ret)
        QIODevice::open(mode);
    return ret;
}
#if 0
bool SaveFileOrStdOut::isValid() const
{
    return m_IsValid;
}
#endif
void SaveFileOrStdOut::setDirectWriteFallback(bool enabled)
{
    //you can set it, but you can't query it (otherwise we'd need to store/sync a local copy of whether or not it's enabled)
    if(!m_IsStdOut)
        static_cast<QSaveFile*>(m_IODeviceBeingProxyingFor)->setDirectWriteFallback(enabled);
}
#if 1 //I inherited QIODevice directly at first, but realized QFileDevice would be better... and then went back to QIODevice because I realized I gained nothing xD
qint64 SaveFileOrStdOut::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)
    qFatal("SaveFileOrStdOut::readData is called");
}
qint64 SaveFileOrStdOut::writeData(const char *data, qint64 len)
{
    return m_IODeviceBeingProxyingFor->write(data, len);
}
#endif
void SaveFileOrStdOut::privateConstructor(const QString &fileName/*, QFlags<OpenModeFlag> openModeFlags*/)
{
    m_FileName = fileName;
    m_IsStdOut = fileName.isEmpty();

    if(m_IsStdOut)
    {
        QFile *stdOutFile = new QFile(this);
        //m_IsValid = stdOutFile->open(stdout, openModeFlags);
        m_IODeviceBeingProxyingFor = stdOutFile;
    }
    else
    {
        QSaveFile *saveFile = new QSaveFile(fileName, this);
        //m_IsValid = saveFile->open(openModeFlags);
        m_IODeviceBeingProxyingFor = saveFile;
    }
    //open(openModeFlags);
}
void SaveFileOrStdOut::close()
{
    qFatal("SaveFileOrStdOut::close is called");
}
