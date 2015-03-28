#include "recursivelyverifycustomdetachedgpgsignatures.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QDirIterator>
#include <QDateTime>

//TODOreq: exclude flag
//TODOoptional: they can pass in a path to recurse (skipping the rest), or hell even a list of paths/files, and of course the opposite holds true: excludes
RecursivelyVerifyCustomDetachedGpgSignatures::RecursivelyVerifyCustomDetachedGpgSignatures(QObject *parent)
    : QObject(parent)
    , m_RecursiveCustomDetachedSignatures(new RecursiveCustomDetachedSignatures(this))
    , m_GpgProcess(new QProcess(this))
    , m_GpgProcessTextStream(m_GpgProcess)
    , m_NumFilesVerified(0)
{
    connect(m_RecursiveCustomDetachedSignatures, SIGNAL(e(QString)), this, SIGNAL(e(QString)));

    m_GpgProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_GpgProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(handleGpgProcessError(QProcess::ProcessError)));
    connect(m_GpgProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(handleGpgProcessFinished(int,QProcess::ExitStatus)));
}
void RecursivelyVerifyCustomDetachedGpgSignatures::buildListOfFilesOnFsToSeeIfAnyAreMissingSigs(const QDir &dir, const QStringList &excludeEntries)
{
    //TODOoptimization: since this app is already asynchronous, it would be "neat" if the building of this list was ALSO asynchronous. basically do one directory at a time, with a QMetaObject::invokeMethod using QQueuedConnection to schedule the generating of the next directory... so that the building of this list, and the verifying of gpg sigs and subsequent removal from this list being asynchronously built, could be done in a MEMORY EFFICIENT manner (if the dir was huuuuuuuuuge, it might actually be noticeable). In that design, when a gpg sig finishes verifying and it isn't seen in this list we are asynchronously building (not because it doesn't exist (it must or the verify will fail), but simply because we haven't gotten to that directory yet), we should put it in a separate list that is checked against before entries are added into this main list being built here ("list of files on filesystem")... and of course once the async fs gen does encounter an already verified file, it simply removes the entry from that additional list

    //A much less memory efficient "synchronous" strategy is being used instead of the above described optimization.... where every filepath in the target dir must be in memory before we can even begin verifying sigs
    m_CharacterLengthOfAbsolutePathOfTargetDir_IncludingTrailingSlash = dir.absolutePath().length() + 1; //+1 to account for trailing slash
    QDirIterator dirIterator(dir.absolutePath(), (QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden), QDirIterator::Subdirectories);
    m_FilesOnFsToSeeIfAnyAreMissingSigsAndToCheckLastModifiedTImestampsAgainst.clear();
    while(dirIterator.hasNext())
    {
        dirIterator.next();
        const QFileInfo &currentEntry = dirIterator.fileInfo();
        if(currentEntry.isFile())
        {
            const QString &currentEntryRelativePath = currentEntry.filePath().mid(m_CharacterLengthOfAbsolutePathOfTargetDir_IncludingTrailingSlash);
            if(excludeEntries.contains(currentEntryRelativePath))
                continue;
            m_FilesOnFsToSeeIfAnyAreMissingSigsAndToCheckLastModifiedTImestampsAgainst.insert(currentEntryRelativePath, (currentEntry.lastModified().toMSecsSinceEpoch() / 1000));
        }
    }
}
void RecursivelyVerifyCustomDetachedGpgSignatures::verifyNextEntryOfCustomDetachedOrEmitFinishedIfNoMore() //pseudo-recursive (async) -- head
{
    if(!m_CustomDetachedSignaturesStream.atEnd())
    {
        QString currentFilePathToVerify;
        QString currentFileSignature;
        qint64 currentFileUnixTimestamp;
        if(!m_RecursiveCustomDetachedSignatures->readPathAndSignature(m_CustomDetachedSignaturesStream, &currentFilePathToVerify, &currentFileSignature, &currentFileUnixTimestamp))
        {
            emit doneRecursivelyVerifyCustomDetachedGpgSignatures(false);
            return;
        }
        m_FileMetaCurrentlyBeingVerified = RecursiveCustomDetachedSignaturesFileMeta(currentFilePathToVerify, currentFileSignature, currentFileUnixTimestamp);

        //TODOoptional: I need to check here/now (well I could check after the sig verifies, but might as well check before since it's cheaper) whether the fs last modified timestamp matches the sigfiles last modified timestamp, because after the sig verify is successful, we remove it from m_FilesOnFsToSeeIfAnyAreMissingSigsAndToCheckLastModifiedTImestampsAgainst. I am going to error out here as soon as a last modified timestamp mismatch is seen, but I could keep a "list of files with mismatching timestamps" and continue onto the end. Similarly, the recursive gpg signer could also not error-out-on-first-timestamp-mismatch and keep building a list and present it at the end. It seems "off" that I'm waiting until the end to show a list of files missing on fs. I guess I could compare the contents of the two lists before verifying even one (and then error out on first non-existent file) just so it matches the functionality of these timestamps mismatches, BUT tbh I can't make up my mind which I like better: error out on first error, or build up errors and present them all at the end in a list. For a huuuuge directory, the first is better. For a small to medium sized directory, the second is better. I've seen plenty of tools that have CLI args for deciding how to handle errors (quit on first error vs. accumulate and continue), so obviously that is the best/proper solution. But what should I default to? I'll tell ya what: whatever is simplest. So for that reason, I'm erroring out here on the first timestamp mismatch
        QHash<QString /*file path*/, qint64 /*last modified unix timestamp in seconds*/>::const_iterator it = m_FilesOnFsToSeeIfAnyAreMissingSigsAndToCheckLastModifiedTImestampsAgainst.find(currentFilePathToVerify);
        if(it != m_FilesOnFsToSeeIfAnyAreMissingSigsAndToCheckLastModifiedTImestampsAgainst.end()) //silently ignore file not existing _here_, because we don't want to ruin the code at the end that spits out a list of missing files -- err actually that list is used to detect files that DO exist on fs but are not in sigs file, my bad. the git verify command itself is what fails if the file isn't on the fs...
        {
            if(!m_RecursiveCustomDetachedSignatures->filesystemLastModifiedUnixTimestampAndMetaUnixTimestampsAreIdentical(it.value(), m_FileMetaCurrentlyBeingVerified))
            {
                emit doneRecursivelyVerifyCustomDetachedGpgSignatures(false);
                return;
            }
        }

        verifyFileSignatureAndThenContinueOntoNextEntryOfCustomDetached();
        return;
    }

    if(!m_FilesOnFsToSeeIfAnyAreMissingSigsAndToCheckLastModifiedTImestampsAgainst.isEmpty())
    {
        emit e("the below files on the filesystem were not in the gpg signatures file:");
        emit e("");
        QHashIterator<QString /*file path*/, qint64 /*last modified unix timestamp in seconds*/> it(m_FilesOnFsToSeeIfAnyAreMissingSigsAndToCheckLastModifiedTImestampsAgainst);
        while(it.hasNext())
        {
            it.next();
            emit e(it.key());
        }
        emit e("");
        emit e("the above files on the filesystem were not in the gpg signatures file");
        emit o("however, every file in the gpg signatures file verified correctly, so it wasn't a complete failure");
        emit doneRecursivelyVerifyCustomDetachedGpgSignatures(false);
        return;
    }

    emit o("done verifying " + QString::number(m_NumFilesVerified) + " custom detached gpg signatures -- everything OK");
    emit doneRecursivelyVerifyCustomDetachedGpgSignatures(true);
}
void RecursivelyVerifyCustomDetachedGpgSignatures::verifyFileSignatureAndThenContinueOntoNextEntryOfCustomDetached()
{
    QStringList gpgArgs;
    gpgArgs << "--verify" << "--batch" << "--no-tty"  << "-" << m_FileMetaCurrentlyBeingVerified.FilePath;
    m_GpgProcess->start(GPG_DEFAULT_PATH, gpgArgs, QIODevice::ReadWrite);
    m_GpgProcessTextStream << m_FileMetaCurrentlyBeingVerified.GpgSignature;
    m_GpgProcessTextStream.flush(); //necessary? probably, but idk tbh
    m_GpgProcess->closeWriteChannel();
}
void RecursivelyVerifyCustomDetachedGpgSignatures::spitOutGpgProcessOutput()
{
    emit e(m_GpgProcessTextStream.readAll());
}
void RecursivelyVerifyCustomDetachedGpgSignatures::recursivelyVerifyCustomDetachedGpgSignatures(const QString &dir, const QString &customDetachedSignaturesFile, const QStringList &excludeEntries)
{
    QDir myDir(dir);
    QFile *myFile = new QFile(customDetachedSignaturesFile, this);
    QFileInfo inputSigFileInfo(*myFile);
    const QString &absolutePathOfDirToRecursivelyVerify_WithSlashAppended = appendSlashIfNeeded(dir);
    const QString &inputSigsFileAbsolutePath = inputSigFileInfo.absoluteFilePath();
    QStringList excludeEntriesWithInputSigsFilePossiblyAddedToExclusionList = excludeEntries; //cow
    if(inputSigsFileAbsolutePath.startsWith(absolutePathOfDirToRecursivelyVerify_WithSlashAppended))
    {
        //input sigs file is in target dir, so exclude it
        QString inputSigsFileRelativePath = inputSigsFileAbsolutePath.mid(absolutePathOfDirToRecursivelyVerify_WithSlashAppended.length());
        excludeEntriesWithInputSigsFilePossiblyAddedToExclusionList << inputSigsFileRelativePath;
    }
    if((!inputSigFileInfo.exists()) || (!inputSigFileInfo.isFile()))
    {
        emit e("sigsfile either doesn't exist or isn't a file: " + customDetachedSignaturesFile);
        emit doneRecursivelyVerifyCustomDetachedGpgSignatures(false);
        return;
    }
    if(!myFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit e("failed to open for reading: " + customDetachedSignaturesFile);
        emit doneRecursivelyVerifyCustomDetachedGpgSignatures(false);
        return;
    }
    recursivelyVerifyCustomDetachedGpgSignatures(myDir, myFile, excludeEntriesWithInputSigsFilePossiblyAddedToExclusionList);
}
void RecursivelyVerifyCustomDetachedGpgSignatures::recursivelyVerifyCustomDetachedGpgSignatures(const QDir &dir, QIODevice *customDetachedSignaturesIoDevice,  const QStringList &excludeEntries)
{
    if(!dir.exists())
    {
        emit e("dir does not exist: " + dir.path());
        emit doneRecursivelyVerifyCustomDetachedGpgSignatures(false);
        return;
    }
    buildListOfFilesOnFsToSeeIfAnyAreMissingSigs(dir, excludeEntries);
    m_CustomDetachedSignaturesStream.setDevice(customDetachedSignaturesIoDevice);
    m_GpgProcess->setWorkingDirectory(dir.absolutePath());
    verifyNextEntryOfCustomDetachedOrEmitFinishedIfNoMore(); //pseudo-recursive (async) -- first head call
}
void RecursivelyVerifyCustomDetachedGpgSignatures::handleGpgProcessError(QProcess::ProcessError processError)
{
    emit e("gpg qprocess error: '" + QString::number(processError) + "' while trying to verify file: " + m_FileMetaCurrentlyBeingVerified.FilePath);
    emit doneRecursivelyVerifyCustomDetachedGpgSignatures(false);
}
void RecursivelyVerifyCustomDetachedGpgSignatures::handleGpgProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus != QProcess::NormalExit)
    {
        spitOutGpgProcessOutput();
        emit e("gpg did not exit normally when trying to verify file: " + m_FileMetaCurrentlyBeingVerified.FilePath);
        emit doneRecursivelyVerifyCustomDetachedGpgSignatures(false);
        return;
    }
    if(exitCode != 0)
    {
        spitOutGpgProcessOutput();
        emit e("one of the signatures was not valid, or there was some other reason gpg did not exit with code 0. gpg exitted with code: " + QString::number(exitCode) + " while trying to verify file: " + m_FileMetaCurrentlyBeingVerified.FilePath); //TODOoptional: this, and other errors, can possibly show the sig that failed to verify... but eh filename is probably enough
        emit doneRecursivelyVerifyCustomDetachedGpgSignatures(false);
        return;
    }

    //read + ignore the gpg output
    m_GpgProcessTextStream.readAll();

    //TODOoptional: if(verbose) { file <name> verified }

    m_FilesOnFsToSeeIfAnyAreMissingSigsAndToCheckLastModifiedTImestampsAgainst.remove(m_FileMetaCurrentlyBeingVerified.FilePath); //is guaranteed to be in there, unless the user is messing with the dir contents while the app is running (which will lead to undefined behavior anyways)
    ++m_NumFilesVerified;
    verifyNextEntryOfCustomDetachedOrEmitFinishedIfNoMore(); //pseudo-recursive (async) -- tail
}
