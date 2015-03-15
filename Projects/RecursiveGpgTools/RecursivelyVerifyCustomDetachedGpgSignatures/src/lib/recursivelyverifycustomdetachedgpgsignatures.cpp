#include "recursivelyverifycustomdetachedgpgsignatures.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QDirIterator>

#include "recursivecustomdetachedgpgsignatures.h"

RecursivelyVerifyCustomDetachedGpgSignatures::RecursivelyVerifyCustomDetachedGpgSignatures(QObject *parent)
    : QObject(parent)
    , m_RecursiveCustomDetachedSignatures(new RecursiveCustomDetachedSignatures(this))
    , m_GpgProcess(new QProcess(this))
    , m_GpgProcessTextStream(m_GpgProcess)
{
    connect(m_RecursiveCustomDetachedSignatures, SIGNAL(e(QString)), this, SIGNAL(e(QString)));

    m_GpgProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_GpgProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(handleGpgProcessError(QProcess::ProcessError)));
    connect(m_GpgProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(handleGpgProcessFinished(int,QProcess::ExitStatus)));
}
void RecursivelyVerifyCustomDetachedGpgSignatures::buildListOfFilesOnFsToSeeIfAnyAreMissingSigs(const QDir &dir)
{
    //TODOoptimization: since this app is already asynchronous, it would be "neat" if the building of this list was ALSO asynchronous. basically do one directory at a time, with a QMetaObject::invokeMethod using QQueuedConnection to schedule the generating of the next directory... so that the building of this list, and the verifying of gpg sigs and subsequent removal from this list being asynchronously built, could be done in a MEMORY EFFICIENT manner (if the dir was huuuuuuuuuge, it might actually be noticeable). In that design, when a gpg sig finishes verifying and it isn't seen in this list we are asynchronously building (not because it doesn't exist (it must or the verify will fail), but simply because we haven't gotten to that directory yet), we should put it in a separate list that is checked against before entries are added into this main list being built here ("list of files on filesystem")... and of course once the async fs gen does encounter an already verified file, it simply removes the entry from that additional list

    //A much less memory efficient "synchronous" strategy is being used instead of the above described optimization.... where every filepath in the target dir must be in memory before we can even begin verifying sigs
    m_CharacterLengthOfAbsolutePathOfTargetDir_IncludingTrailingSlash = dir.absolutePath().length() + 1; //+1 to account for trailing slash
    QDirIterator dirIterator(dir.absolutePath(), (QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden), QDirIterator::Subdirectories);
    m_ListOfFileOnFsToSeeIfAnyAreMissingSigs.clear();
    while(dirIterator.hasNext())
    {
        dirIterator.next();
        const QFileInfo &currentEntry = dirIterator.fileInfo();
        if(currentEntry.isFile())
        {
            m_ListOfFileOnFsToSeeIfAnyAreMissingSigs.insert(currentEntry.filePath().mid(m_CharacterLengthOfAbsolutePathOfTargetDir_IncludingTrailingSlash));
        }
    }
}
void RecursivelyVerifyCustomDetachedGpgSignatures::verifyNextEntryOfCustomDetachedOrEmitFinishedIfNoMore() //pseudo-recursive (async) -- head
{
    if(!m_CustomDetachedSignaturesStream.atEnd())
    {
        QString currentFilePathToVerify;
        QString currentFileSignature;
        if(!m_RecursiveCustomDetachedSignatures->readPathAndSignature(m_CustomDetachedSignaturesStream, &currentFilePathToVerify, &currentFileSignature))
        {
            emit doneRecursivelyVerifyCustomDetachedGpgSignatures(false);
            return;
        }
        verifyFileSignatureAndThenContinueOntoNextEntryOfCustomDetached(currentFilePathToVerify, currentFileSignature);
        return;
    }

    if(!m_ListOfFileOnFsToSeeIfAnyAreMissingSigs.isEmpty())
    {
        emit e("the below files on the filesystem were not in the gpg signature file:");
        emit e("");
        Q_FOREACH(const QString &currentFileOnFsButNotInSigFile, m_ListOfFileOnFsToSeeIfAnyAreMissingSigs)
        {
            emit e(currentFileOnFsButNotInSigFile);
        }
        emit e("");
        emit e("the above files on the filesystem were not in the gpg signature file");
        emit o("however, every file in the gpg signature file did verify, so it wasn't a complete failure");
        emit doneRecursivelyVerifyCustomDetachedGpgSignatures(false);
        return;
    }

    emit o("done verifying custom detached gpg signatures -- everything OK");
    emit doneRecursivelyVerifyCustomDetachedGpgSignatures(true);
}
void RecursivelyVerifyCustomDetachedGpgSignatures::verifyFileSignatureAndThenContinueOntoNextEntryOfCustomDetached(const QString &filePathToVerify, const QString &fileSignature)
{
    m_FilePathCurrentlyBeingVerified = filePathToVerify;

    QStringList gpgArgs;
    gpgArgs << "--verify" << "-" << filePathToVerify;
    m_GpgProcess->start(GPG_DEFAULT_PATH, gpgArgs, QIODevice::ReadWrite);
    m_GpgProcessTextStream << fileSignature;
    m_GpgProcessTextStream.flush(); //necessary? probably, but idk tbh
    m_GpgProcess->closeWriteChannel();
}
void RecursivelyVerifyCustomDetachedGpgSignatures::spitOutGpgProcessOutput()
{
    emit e(m_GpgProcessTextStream.readAll());
}
void RecursivelyVerifyCustomDetachedGpgSignatures::recursivelyVerifyCustomDetachedGpgSignatures(const QString &dir, const QString &customDetachedSignaturesFile)
{
    QDir myDir(dir);
    QFile *myFile = new QFile(customDetachedSignaturesFile, this);
    if(!myFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit e("failed to open for reading: " + customDetachedSignaturesFile);
        emit doneRecursivelyVerifyCustomDetachedGpgSignatures(false);
        return;
    }
    recursivelyVerifyCustomDetachedGpgSignatures(myDir, myFile);
}
void RecursivelyVerifyCustomDetachedGpgSignatures::recursivelyVerifyCustomDetachedGpgSignatures(const QDir &dir, QIODevice *customDetachedSignaturesIoDevice)
{
    if(!dir.exists())
    {
        emit e("dir does not exist: " + dir.path());
        emit doneRecursivelyVerifyCustomDetachedGpgSignatures(false);
        return;
    }
    buildListOfFilesOnFsToSeeIfAnyAreMissingSigs(dir);
    m_CustomDetachedSignaturesStream.setDevice(customDetachedSignaturesIoDevice);
    m_GpgProcess->setWorkingDirectory(dir.absolutePath());
    verifyNextEntryOfCustomDetachedOrEmitFinishedIfNoMore(); //pseudo-recursive (async) -- first head call
}
void RecursivelyVerifyCustomDetachedGpgSignatures::handleGpgProcessError(QProcess::ProcessError processError)
{
    emit e("gpg qprocess error: '" + QString::number(processError) + "' while trying to verify file: " + m_FilePathCurrentlyBeingVerified);
    emit doneRecursivelyVerifyCustomDetachedGpgSignatures(false);
}
void RecursivelyVerifyCustomDetachedGpgSignatures::handleGpgProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus != QProcess::NormalExit)
    {
        spitOutGpgProcessOutput();
        emit e("gpg did not exit normally when trying to verify file: " + m_FilePathCurrentlyBeingVerified);
        emit doneRecursivelyVerifyCustomDetachedGpgSignatures(false);
        return;
    }
    if(exitCode != 0)
    {
        spitOutGpgProcessOutput();
        emit e("one of the signatures was not valid, or there was some other reason gpg did not exit with code 0. gpg exitted with code: " + QString::number(exitCode) + " while trying to verify file: " + m_FilePathCurrentlyBeingVerified); //TODOoptional: this, and other errors, can possibly show the sig that failed to verify... but eh filename is probably enough
        emit doneRecursivelyVerifyCustomDetachedGpgSignatures(false);
        return;
    }

    //read + ignore the gpg output
    m_GpgProcessTextStream.readAll();

    //TODOoptional: if(verbose) { file <name> verified }

    m_ListOfFileOnFsToSeeIfAnyAreMissingSigs.remove(m_FilePathCurrentlyBeingVerified); //is guaranteed to be in there, unless the user is messing with the dir contents while the app is running (which will lead to undefined behavior anyways)
    verifyNextEntryOfCustomDetachedOrEmitFinishedIfNoMore(); //pseudo-recursive (async) -- tail
}
