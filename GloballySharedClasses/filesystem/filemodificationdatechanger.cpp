#include "filemodificationdatechanger.h"

const QString FileModificationDateChanger::m_TouchProcessPath = "/bin/touch";
FileModificationDateChanger::FileModificationDateChanger(QObject *parent) :
    QObject(parent)
{
    m_TouchProcess.setProcessChannelMode(QProcess::MergedChannels);
}
bool FileModificationDateChanger::changeModificationDate(const QString &absolutePathOfFileToTouch, QDateTime newDateTime)
{
#if 0 //this hack is still here, but i've moved it to heirarchy molester's 'load' functions (both of them)
    if(absolutePathOfFileToTouch.endsWith("/")) //re-create dirs that weren't committed to git because no files inside. I get the sneaking suspicion that this hack is going to bite me in the future somehow. Hello future pissed off self
    {
        if(!QFile::exists(absolutePathOfFileToTouch))
        {
            QDir dir(absolutePathOfFileToTouch);
            if(!dir.mkpath(absolutePathOfFileToTouch))
            {
                emit d("while trying to make a dir (that was empty so git missed it) before touching, we failed");
                return false;
            }
        }
    }
#endif

    QStringList touchArguments;
    touchArguments << "-m" << QString("--date=@" + QString::number(newDateTime.toMSecsSinceEpoch()/1000)) << absolutePathOfFileToTouch;
    m_TouchProcess.start(m_TouchProcessPath, touchArguments);
    if(!m_TouchProcess.waitForStarted())
    {
        emit d("failed to start touch executable");
        return false;
    }
    if(!m_TouchProcess.waitForFinished())
    {
        emit d("touch executable failed to finish");
        return false;
    }
    bool ret = true;
    int exitCode = m_TouchProcess.exitCode();
    if(exitCode != 0)
    {
        emit d("touch exit code was not zero");
        ret = false;
    }
    QString touchResultString = m_TouchProcess.readAll();
    if(!touchResultString.trimmed().isEmpty())
    {
        emit d("touch said something on stdout/stderr, so there was probably an error:");
        emit d(touchResultString);
        ret = false;
    }
    return ret;
}
