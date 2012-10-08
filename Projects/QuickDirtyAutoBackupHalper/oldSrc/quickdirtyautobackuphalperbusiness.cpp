#include "quickdirtyautobackuphalperbusiness.h"

QuickDirtyAutoBackupHalperBusiness::QuickDirtyAutoBackupHalperBusiness(QObject *parent) :
    QObject(parent), m_StringTcLocationsAndPwFilesArrayName("tcContainerAndPwFileLocations"), m_StringContainerLocationKeyName("container"), m_StringPasswordLocationKeyName("passwordFile"), m_MutexProtectedIdleFlag(true), m_MutexProtectedDoneFlag(false)
{
}
QMutex *QuickDirtyAutoBackupHalperBusiness::getWorkerMutex()
{
    return &m_WorkerMutex;
}
bool QuickDirtyAutoBackupHalperBusiness::isIdle()
{
    //mutex needs to be locked by outside thread before calling...
    return m_MutexProtectedIdleFlag;
}
bool QuickDirtyAutoBackupHalperBusiness::setDone(bool done)
{
    //mutex needs to be locked by outside thread before calling...
    m_MutexProtectedDoneFlag = done;
}
void QuickDirtyAutoBackupHalperBusiness::readSettingsAndProbeMountStatus()
{
    QSettings settings;
    int arraySize = settings.beginReadArray(m_StringTcLocationsAndPwFilesArrayName);
    for(int i = 0; i < arraySize; ++i)
    {
        settings.setArrayIndex(i);
        TcContainerPathAndPasswordPathValues tcValues;
        tcValues.ContainerLocation = settings.value(m_StringContainerLocationKeyName).toString();
        tcValues.PasswordLocation = settings.value(m_StringPasswordLocationKeyName).toString();

        //sanitize input. make sure the strings are not empty and the files exist
        if(tcValues.ContainerLocation.trimmed().isEmpty() || tcValues.PasswordLocation.trimmed().isEmpty())
        {
            continue;
        }
        else
        {
            //strings aren't empty

            if(existsAndIsntDirOrSymlink(tcValues.ContainerLocation) && existsAndIsntDirOrSymlink(tcValues.PasswordLocation))
            {
                //add to our internal list, and emit that list to the GUI perhaps? wait no i need to make sure either all of them are mounted or dismounted (no mixing!) before telling the gui. maybe the list i append to here is temporary/private (still needs testing/probing)
                m_ReadInFromSettingsContainerAndPasswordPaths.append(tcValues);
            }
        }
    }
    settings.endArray();

    probeMountStatus();
}
void QuickDirtyAutoBackupHalperBusiness::probeMountStatus()
{
    QMutexLocker scopedLock(&m_WorkerMutex);
    if(m_MutexProtectedIdleFlag) //TODO: maybe an exception to this to see if we're in the pre-commit stages (verifying the mount status _JIT_ before commit. might also need to pick back up on that commit after the probe completes (gah async makes shit hard as fuck sometimes))
    {
        if(!m_MutexProtectedDoneFlag)
        {
            m_MutexProtectedIdleFlag = false; //not idle anymore, we're probing

            //TODO: probe etc
        }
#if 0
        else
        {
            //eh we're done... which actually means our thread has been quit'd/wait'd... so we can't emit a signal saying we got here because it'd never be delivered (maybe i'm wrong idfk)
        }
#endif
    }
    else
    {
        emit d("probe was called in the business, but we are not idle so we're ignoring it");
    }
}
bool QuickDirtyAutoBackupHalperBusiness::existsAndIsntDirOrSymlink(QString filePath)
{
    m_FileInfo.setFile(filePath);
    if(m_FileInfo.exists() && m_FileInfo.isFile())
    {
        if(!m_FileInfo.isSymLink()) //can't handle symlinks i don't think (could be wrong)
        {
            return true;
        }
    }
    return false;
}
void QuickDirtyAutoBackupHalperBusiness::start()
{
    //QProcess connections init'ing etc

    readSettingsAndProbeMountStatus();
}
void QuickDirtyAutoBackupHalperBusiness::mount()
{
    QMutexLocker scopedLock(&m_WorkerMutex);
    if(m_MutexProtectedIdleFlag)
    {
        if(!m_MutexProtectedDoneFlag)
        {
            m_MutexProtectedIdleFlag = false; //not idle anymore, we're mounting :)

            //TODO: mount etc
        }
#if 0
        else
        {
            //eh we're done... which actually means our thread has been quit'd/wait'd... so we can't emit a signal saying we got here because it'd never be delivered (maybe i'm wrong idfk)
        }
#endif
    }
    else
    {
        emit d("mount was called in the business, but we are not idle so we're ignoring it");
    }
}
