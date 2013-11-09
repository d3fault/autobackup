#include "filemodificationdatechanger.h"

const QString FileModificationDateChanger::m_TouchProcessPath = "/bin/touch";
FileModificationDateChanger::FileModificationDateChanger(QObject *parent) :
    QObject(parent)
{
    m_TouchProcess.setProcessChannelMode(QProcess::MergedChannels);
}
bool FileModificationDateChanger::changeModificationDate(const QString &absolutePathOfFileToTouch, QDateTime newDateTime)
{
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

    int exitCode = m_TouchProcess.exitCode();
    if(exitCode != 0)
    {
        emit d("touch exit code was not zero");
        return false;
    }

    QString touchResultString = m_TouchProcess.readAll();
    if(!touchResultString.trimmed().isEmpty())
    {
        emit d("touch said something on stdout/stderr, so there was probably an error");
        return false;
    }
    return true;
}
