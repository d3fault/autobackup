6#include "quickdirtyautobackuphalper.h"

QuickDirtyAutoBackupHalper::QuickDirtyAutoBackupHalper(QObject *parent) :
    QObject(parent), /*m_ProcIsTruecrypt(true),*/ m_OneErrorWasDetected(false)
{ }
QuickDirtyAutoBackupHalper::~QuickDirtyAutoBackupHalper()
{
    delete m_AllProcessOutput;
    delete m_MySubProcess;
}
QString QuickDirtyAutoBackupHalper::currentProcessNameHumanReadable()
{
    QString ret;
    switch(m_WtfAmIDoing)
    {
    case ProbingMountStatus:
        ret.append("ProbingTcMount");
        break;
    case MountingTruecrypt:
        ret.append("MountingTruecrypt");
        break;
    case DismountingTruecrypt:
        ret.append("DismountingTruecrypt");
        break;
    case TreeingDirStructure:
        ret.append("TreeingDirStructure");
        break;
    case Committing:
        ret.append("Committing");
        break;
    case DoingNothingOrInvalid:
    default:
        ret.append("InvalidDoingNothingDefault");
        break;
    }
    return ret;
}
void QuickDirtyAutoBackupHalper::processTheOutputBasedOnWhatImDoing()
{
    //output it no matter what
    QString spatOut;
    spatOut.append(" spat out: ");
    emit d(QString(currentProcessNameHumanReadable() + spatOut + *m_AllProcessOutput));

    //sanity check. it should really be checked BEFORE the process is executed
    if(m_OneErrorWasDetected)
        return;

    switch(m_WtfAmIDoing)
    {
        case ProbingMountStatus:
        {
#if 0
        d3fault@debianHostname:~$ /usr/bin/sudo /usr/bin/truecrypt -t --verbose --non-interactive -l
        Slot: 9
        Volume: /home/d3fault/Temp/testUserTcContainer
        Virtual Device: /dev/mapper/truecrypt9
        Mount Directory: /media/truecrypt9
        Size: 24.8 MB
        Type: Normal
        Read-Only: No
        Hidden Volume Protected: No
        Encryption Algorithm: AES
        Primary Key Size: 256 bits
        Secondary Key Size (XTS Mode): 256 bits
        Block Size: 128 bits
        Mode of Operation: XTS
        PKCS-5 PRF: HMAC-RIPEMD-160
        Volume Format Version: 2
        Embedded Backup Header: Yes

        d3fault@debianHostname:~$ /usr/bin/sudo /usr/bin/truecrypt -t --non-interactive -l
        9: /home/d3fault/Temp/testUserTcContainer /dev/mapper/truecrypt9 /media/truecrypt9
#endif
            if(m_AllProcessOutput->contains(QString("Error:"), Qt::CaseInsensitive))
            {
                bool origErrorState = m_OneErrorWasDetected;
                m_OneErrorWasDetected = true;
                if(m_AllProcessOutput->contains(QString("No volumes mounted."), Qt::CaseInsensitive) && m_AllProcessOutput->count(QString("Error:"), Qt::CaseInsensitive) == 1)
                {
                    m_OneErrorWasDetected = false; //JK LOL. "Error" is ok in this case. it's the standard way of saying we don't have any mounted. but we do make sure that "Error" only appears once. if it appears more than once then something else went wrong too...
                }
                emit initializedAndProbed(m_OneErrorWasDetected, false);
                m_OneErrorWasDetected = origErrorState;
            }
            else
            {
                //TODOreq: verify that every relevant container/device is either mounted or dismounted (no combinations! else we set fucked=true). also extract the relevant mounted path (/media/truecrypt9 in above example) to use as base path for what we "git push" to
                //SHIT, how am I supposed to know what containers/devices are relevant if at this point we have not even initialized the GUI... and I want to allow the user to specify an arbitrarily length'd list of containers/devices (and passwords)
                emit initializedAndProbed(true, false);
            }
        }
        break;
        case MountingTruecrypt:
        {
            if(m_AllProcessOutput->contains(QString("Error:"), Qt::CaseInsensitive))
            {
                m_OneErrorWasDetected = true;
            }
            else if(m_AllProcessOutput->contains(QString("has been mounted."), Qt::CaseInsensitive))
            {
                emit mounted();
            }
        }
        break;
        case DismountingTruecrypt:
        {
            if(m_AllProcessOutput->contains(QString("Error:"), Qt::CaseInsensitive))
            {
                m_OneErrorWasDetected = true;
            }
            else if(m_AllProcessOutput->contains(QString("has been dismounted."), Qt::CaseInsensitive))
            {
                emit dismounted();
            }
        }
        break;
        case TreeingDirStructure:
        {

        }
        break;
        case Committing:
        {

        }
        break;
        case DoingNothingOrInvalid:
        default:
        {
            m_OneErrorWasDetected = true;
            emit d("somehow got done with a process that is doing nothing/invalid/switch-default'd' wtf?");
        }
        break;
    }

    m_WtfAmIDoing = DoingNothingOrInvalid;
    m_AllProcessOutput->clear(); //clear it for next process
}
void QuickDirtyAutoBackupHalper::initializeAndProbe()
{
    if(!m_OneErrorWasDetected)
    {
        m_MySubProcess = new QProcess(this);
        m_AllProcessOutput = new QString();
        m_WtfAmIDoing = DoingNothingOrInvalid;
        m_MySubProcess->setProcessChannelMode(QProcess::MergedChannels);
        connect(m_MySubProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(handleProcessError(QProcess::ProcessError)));
        connect(m_MySubProcess, SIGNAL(started()), this, SLOT(handleProcessStarted()));
        //connect(m_MySubProcess, SIGNAL(readyRead()), this, SLOT(handleProcessReadyRead()));
        connect(m_MySubProcess, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(handleProcessStateChanged(QProcess::ProcessState)));
        connect(m_MySubProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(handleProcessFinished(int,QProcess::ExitStatus)));

        m_WtfAmIDoing = ProbingMountStatus;
        QString probeCommand("/usr/bin/sudo /usr/bin/truecrypt -t --verbose --non-interactive -l");
        m_MySubProcess->start(probeCommand);
    }
}
void QuickDirtyAutoBackupHalper::mount(bool mountAsReadOnly)
{
    if(!m_OneErrorWasDetected)
    {
        m_WtfAmIDoing = MountingTruecrypt;

        QString mountCommand("/usr/bin/sudo /usr/bin/truecrypt -t --password=fuckyou --slot=9 --verbose --non-interactive /home/d3fault/Temp/testUserTcContainer");
        if(mountAsReadOnly)
        {
            mountCommand.append(" -m ro");
        }

        m_MySubProcess->start(mountCommand);
    }
}
void QuickDirtyAutoBackupHalper::dismount()
{
    if(!m_OneErrorWasDetected)
    {
        m_WtfAmIDoing = DismountingTruecrypt;
    }
}
void QuickDirtyAutoBackupHalper::commit(const QString &commitMsg)
{
    if(!m_OneErrorWasDetected)
    {
        //TODOopt: assumption: we are already mounted. might be wise to verify this yet again... to catch the case wehre the user launches the app, then launches the truecrypt GUI + dismounts etc. we could still use the same function(s) as the initializedAndProbed stuff.... just call it AGAIN.

        //TODOreq: dir.structure.txt -- and it shouldn't see itself (infinite 'changes' loop)


        //TODOreq: do 2x "git logs" and make sure they definitely got in. compare the dates to the "git log" of the autobackup dir/working dir -- and compare the autobackup's git log **MESSAGE** against the commitMsg that was passed to us. we have to verify the msg before we can verify the 2x git logs


        //FUCK MY LIFE, debian doesn't include 'tree'. it's a package. rofl. so should I install it and add it to my list of initial install items... or just code the same functionality again in Qt.
        //a simple:
        /*
        path/To/file1:3923480:0238498208
        path/To/file2:sizeInBytes:unixTime
        */
        //line by line thing would be pretty fuckin easy to code.
        //also i could add my filters to ignore .git directories and others.... (and of course, ignore the dir.structure.txt file i generate!) easily
        //oh and also .pro.uesr files fuck those too :)
        //i could send in a --ignore-filter=gitFilesAndProUserFiles.txt to the command line for a hypothetical tree replacement

        //this is something that is a) easy to code, b) i've been thinking about/wanting to code for a long time, c) eh there is no c. it's vital/will-be-used-in-the-future, etc

        //i like that i will soon/eventually be committing this committer (my offline/tc drives are physically disconnected as i code this -- just in case! using a 20mb tc test container for now (oshi- container vs device problem needs to be tackled fosho. perhaps truecrypt can do the heavy lifting <3)). my brain feels good man (and by that i mean it hurts (and by that i only mean that i am using it))


        //TODOreq: if i do code it, make sure as fuck that it sees hidden files :)
        //hey idiot don't procrastinate this too much even though it SEEMS like a giant task (because it IS if you think within the scope of d3fault... and what that project has become in your brain/hdd)... but that simple filename:filesize:timestamp prog would really be like ~100 lines lmfao. don't procrastinate and think about this for days on end like you have been doing with that project that's on hold until you finish this project so you CAN FUCKING MOUNT YOUR GOD DAMN TC CONTAINERS. lmfao. my life. YATx10000000

        emit committed();


        ------- breaking this motherfucking code because it's unstable and imabout to rewrite with the backend loading/probing and not depending on GUI xD

    }
}
/*
void QuickDirtyAutoBackupHalper::shutdown()
{
    //we dgaf if there was an error...
    QProcess::startDetached("/usr/bin/sudo /sbin/poweroff");
}
*/
bool QuickDirtyAutoBackupHalper::isBusyWithSomething()
{
    if(m_WtfAmIDoing == DoingNothingOrInvalid)
    {
        return false;
    }
    return true;
}
void QuickDirtyAutoBackupHalper::handleProcessError(QProcess::ProcessError error)
{
    m_OneErrorWasDetected = true;
    emit d(currentProcessNameHumanReadable() + QString("error'd out with QProcess::ProcessError = ") + QString::number(error)); //very unlikely 'tree' will error out....
}
void QuickDirtyAutoBackupHalper::handleProcessStarted()
{
    emit d(currentProcessNameHumanReadable() + QString(" has started"));
}
/*void QuickDirtyAutoBackupHalper::handleProcessReadyRead()
{
    emit d(currentProcessNameHumanReadable() + QString(": ") + QString(m_MySubProcess->readAll()));
}*/
void QuickDirtyAutoBackupHalper::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    emit d(currentProcessNameHumanReadable() + QString(" has exited with code: ") + QString::number(exitCode));
    if(exitCode != 0 && m_WtfAmIDoing != ProbingMountStatus) //probing mount status returns as error when none are mounted. but that's ok. we do check later that there isn't in fact ANOTHER error
    {
        m_OneErrorWasDetected = true;
    }
    if(exitStatus == QProcess::CrashExit)
    {
        m_OneErrorWasDetected = true;
        emit d(currentProcessNameHumanReadable() + QString(" has apparently crashed."));
    }
    else
    {
        emit d(currentProcessNameHumanReadable() + QString(" has exited normally"));
        m_AllProcessOutput->append(m_MySubProcess->readAll());
        processTheOutputBasedOnWhatImDoing();
    }
}
void QuickDirtyAutoBackupHalper::handleProcessStateChanged(QProcess::ProcessState newState)
{
    QString outputLoL(currentProcessNameHumanReadable() + QString(" has changed it's process state to: "));
    switch(newState)
    {
    case QProcess::NotRunning:
        outputLoL.append("Not Running");
        break;
    case QProcess::Starting:
        outputLoL.append("Starting");
        break;
    case QProcess::Running:
        outputLoL.append("Running");
        break;
    default:
        outputLoL.append("lol wut");
        break;
    }
    emit d(outputLoL);
}
