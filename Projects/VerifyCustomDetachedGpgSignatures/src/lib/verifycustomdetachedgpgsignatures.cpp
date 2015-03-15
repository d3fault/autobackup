#include "verifycustomdetachedgpgsignatures.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QDirIterator>

#define GPG_END_SIG_DELIMITER "-----END PGP SIGNATURE-----"
#define GPG_DEFAULT_PATH "/usr/bin/gpg" //TODOoptional: custom path as arg

//TODOreq: the sig file generator (which as of now is just a bash script) should be able to "recursively sign heirarchy, skipping already signed files". as of now, it would re-sign the files
//TODOreq: might be a good idea to mention existing files on filesystem that don't have an entry in the signatures file
VerifyCustomDetachedGpgSignatures::VerifyCustomDetachedGpgSignatures(QObject *parent)
    : QObject(parent)
    , m_GpgProcess(new QProcess(this))
    , m_GpgProcessTextStream(m_GpgProcess)
{
    m_GpgProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_GpgProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(handleGpgProcessError(QProcess::ProcessError)));
    connect(m_GpgProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(handleGpgProcessFinished(int,QProcess::ExitStatus)));
}
void VerifyCustomDetachedGpgSignatures::buildListOfFilesOnFsToSeeIfAnyAreMissingSigs(const QDir &dir)
{
    //TODOoptimization: since this app is already asynchronous, it would be "neat" if the building of this list was ALSO asynchronous. basically do one directory at a time, with a QMetaObject::invokeMethod using QQueuedConnection to schedule the generating of the next directory... so that the building of this list, and the verifying of gpg sigs and subsequent removal from this list being asynchronously built, could be done in a MEMORY EFFICIENT manner (if the dir was huuuuuuuuuge, it might actually be noticeable). In that design, when a gpg sig finishes verifying and it isn't seen in this list we are asynchronously building (not because it doesn't exist (it must or the verify will fail), but simply because we haven't gotten to that directory yet), we should put it in a separate list that is checked against before entries are added into this main list being built here ("list of files on filesystem")... and of course once the async fs gen does encounter an already verified file, it simply removes the entry from that additional list

    //A much less memory efficient "synchronous" strategy is being used instead of the above described optimization.... where every filepath in the target dir must be in memory before we can even begin verifying sigs
    QDir dirToIterate = dir;
    dirToIterate.setFilter(dirToIterate.filter() | QDir::NoDotAndDotDot);
    QDirIterator dirIterator(dir, QDirIterator::Subdirectories);
    m_ListOfFileOnFsToSeeIfAnyAreMissingSigs.clear();
    while(dirIterator.hasNext())
    {
        dirIterator.next();
        const QFileInfo &currentEntry = dirIterator.fileInfo();
        if(currentEntry.isFile())
        {
            m_ListOfFileOnFsToSeeIfAnyAreMissingSigs.insert(currentEntry.filePath());
        }
    }
}
void VerifyCustomDetachedGpgSignatures::verifyNextEntryOfCustomDetachedOrEmitFinishedIfNoMore() //pseudo-recursive (async) -- head
{
    if(!m_CustomDetachedSignaturesStream.atEnd())
    {
        QString currentFilePathToVerify;
        QString currentFileSignature;
        if(!readPathAndSignature(&currentFilePathToVerify, &currentFileSignature))
        {
            emit doneVerifyingCustomDetachedGpgSignatures(false);
            return;
        }
        verifyFileSignatureAndThenContinueOntoNextEntryOfCustomDetached(currentFilePathToVerify, currentFileSignature);
        return;
    }

    if(!m_ListOfFileOnFsToSeeIfAnyAreMissingSigs.isEmpty())
    {
        emit e("these files on the filesystem were not in the gpg signature file:");
        emit e("");
        Q_FOREACH(const QString &currentFileOnFsButNotInSigFile, m_ListOfFileOnFsToSeeIfAnyAreMissingSigs)
        {
            emit e(currentFileOnFsButNotInSigFile);
        }
        emit o("");
        emit o("however, every file in the gpg signature file did verify");
        emit doneVerifyingCustomDetachedGpgSignatures(false);
        return;
    }

    emit o("done verifying custom detached gpg signatures");
    emit doneVerifyingCustomDetachedGpgSignatures(true);
}
bool VerifyCustomDetachedGpgSignatures::readPathAndSignature(QString *out_FilePathToVerify, QString *out_CurrentFileSignature)
{
    *out_FilePathToVerify = m_CustomDetachedSignaturesStream.readLine();
    QString lastReadLine;
    do
    {
        if(m_CustomDetachedSignaturesStream.atEnd())
        {
            emit e("malformed custom detached gpg signature file");
            return false;
        }
        lastReadLine = m_CustomDetachedSignaturesStream.readLine();
        out_CurrentFileSignature->append(lastReadLine + "\n"); //TODOoptional: might be better to not use readLine up above and to just read until the string is seen (retain whatever newline form the output of gpg gave us to begin with)
    }
    while(lastReadLine != GPG_END_SIG_DELIMITER);
    return true;
}
void VerifyCustomDetachedGpgSignatures::verifyFileSignatureAndThenContinueOntoNextEntryOfCustomDetached(const QString &filePathToVerify, const QString &fileSignature)
{
    m_FilePathCurrentlyBeingVerified = filePathToVerify;

    QStringList gpgArgs;
    gpgArgs << "--verify" << "-" << filePathToVerify;
    m_GpgProcess->start(GPG_DEFAULT_PATH, gpgArgs, QIODevice::ReadWrite);
    m_GpgProcessTextStream << fileSignature;
    m_GpgProcessTextStream.flush(); //necessary? probably, but idk tbh
    m_GpgProcess->closeWriteChannel();
}
void VerifyCustomDetachedGpgSignatures::spitOutGpgProcessOutput()
{
    emit e(m_GpgProcessTextStream.readAll());
}
void VerifyCustomDetachedGpgSignatures::verifyCustomDetachedGpgSignatures(const QString &dir, const QString &customDetachedSignaturesFile)
{
    QDir myDir(dir);
    QFile myFile(customDetachedSignaturesFile);
    if(!myFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit e("failed to open for reading: " + customDetachedSignaturesFile);
        emit doneVerifyingCustomDetachedGpgSignatures(false);
        return;
    }
    verifyCustomDetachedGpgSignatures(myDir, &myFile);
}
void VerifyCustomDetachedGpgSignatures::verifyCustomDetachedGpgSignatures(const QDir &dir, QIODevice *customDetachedSignaturesIoDevice)
{
    if(!dir.exists())
    {
        emit e("dir does not exist: " + dir.path());
        emit doneVerifyingCustomDetachedGpgSignatures(false);
        return;
    }

    buildListOfFilesOnFsToSeeIfAnyAreMissingSigs(dir);

    m_CustomDetachedSignaturesStream.setDevice(customDetachedSignaturesIoDevice);
    m_GpgProcess->setWorkingDirectory(dir.absolutePath());
    verifyNextEntryOfCustomDetachedOrEmitFinishedIfNoMore(); //pseudo-recursive (async) -- first head call
}
void VerifyCustomDetachedGpgSignatures::handleGpgProcessError(QProcess::ProcessError processError)
{
    emit e("gpg qprocess error: '" + QString::number(processError) + "' while trying to verify file: " + m_FilePathCurrentlyBeingVerified);
    emit doneVerifyingCustomDetachedGpgSignatures(false); //TODOoptional: this and handleGpgProcessFinished might both emit this signal. probably doesn't matter
}
void VerifyCustomDetachedGpgSignatures::handleGpgProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus != QProcess::NormalExit)
    {
        spitOutGpgProcessOutput();
        emit e("gpg did not exit normally when trying to verify file: " + m_FilePathCurrentlyBeingVerified);
        emit doneVerifyingCustomDetachedGpgSignatures(false);
        return;
    }
    if(exitCode != 0)
    {
        spitOutGpgProcessOutput();
        emit e("one of the signatures was not valid, or there was some other reason gpg did not exit with code 0. gpg exitted with code: " + QString::number(exitCode) + " while trying to verify file: " + m_FilePathCurrentlyBeingVerified); //TODOoptional: this, and other errors, can possibly show the sig that failed to verify... but eh filename is probably enough
        emit doneVerifyingCustomDetachedGpgSignatures(false);
        return;
    }

    //read + ignore the gpg output
    m_GpgProcessTextStream.readAll();

    //TODOoptional: if(verbose) { file <name> verified }

    m_FilePathCurrentlyBeingVerified.remove(m_FilePathCurrentlyBeingVerified); //is guaranteed to be in there, unless the user is messing with the dir contents while the app is running (which will lead to undefined behavior anyways)
    verifyNextEntryOfCustomDetachedOrEmitFinishedIfNoMore(); //pseudo-recursive (async) -- tail
}
