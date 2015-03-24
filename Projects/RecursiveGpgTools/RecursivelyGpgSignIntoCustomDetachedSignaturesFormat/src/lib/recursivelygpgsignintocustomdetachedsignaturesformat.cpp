#include "recursivelygpgsignintocustomdetachedsignaturesformat.h"

#include <QFile>
#include <QDir>
#include <QHashIterator>
#include <QMapIterator>
#include <QDateTime>

//TODOoptional: make this app write to stdout if no output sigsfile arg is specified. similarly, the verifier should read from stdin if no input sigsfile arg is specified. directory is probably always necessary, HOWEVER it might be good to default to cwd for both this and the verifier when none is provided?
//TODOoptional: a combination of sign+verify: sign files on fs not in sigfile, verify files in sigfile, report sigs in sigfile that do not exist on filesystem -- it shouldn't be the default because reading/verifying EVERY file would take a long as fuck time
//TODOoptimization: applies to both this and verifier. if i'm not IO bound but am in fact cpu bound, then QtConcurrent could possibly be used... so long as gpg doesn't care about being called multiple times simultaneously
RecursivelyGpgSignIntoCustomDetachedSignaturesFormat::RecursivelyGpgSignIntoCustomDetachedSignaturesFormat(QObject *parent)
    : QObject(parent)
    , m_RecursiveCustomDetachedSignatures(new RecursiveCustomDetachedSignatures(this))
    , m_GpgProcess(new QProcess(this))
    , m_GpgProcessTextStream(m_GpgProcess)
    , m_ExistingSigs(0)
    , m_AddedSigs(0)
    , m_TotalSigs(0)
{
    connect(m_RecursiveCustomDetachedSignatures, SIGNAL(e(QString)), this, SIGNAL(e(QString)));

    connect(m_GpgProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(handleGpgProcessError(QProcess::ProcessError)));
    connect(m_GpgProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(handleGpgProcessFinished(int,QProcess::ExitStatus)));
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormat::readInAllSigsFromSigFileSoWeKnowWhichOnesToSkipAsWeRecurseTheFilesystem()
{
    if(!m_InputAndOutputSigFileTextStream.device()->isOpen()) //not open == doesn't exist yet
        return;

    while(!m_InputAndOutputSigFileTextStream.atEnd())
    {
        QString alreadySignedFilePath;
        QString alreadySignedFileSig;
        qint64 alreadySignedFileUnixTimestamp;
        if(!m_RecursiveCustomDetachedSignatures->readPathAndSignature(m_InputAndOutputSigFileTextStream, &alreadySignedFilePath, &alreadySignedFileSig, &alreadySignedFileUnixTimestamp))
        {
            emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(false);
            return;
        }
        m_AllSigsFromSigFileSoWeKnowWhichOnesToSkipAsWeRecurseTheFilesystem.insert(alreadySignedFilePath, RecursiveCustomDetachedSignaturesFileMeta(alreadySignedFilePath, alreadySignedFileSig, alreadySignedFileUnixTimestamp)); //TODOreq: we start off with a hash, but as their existences are verified, we move them into a map... the same map that new files+sigs are being placed into. we want it to be sorted for when we re-write the signature file with the new entries. TODOoptimization: don't re-write the sigs file if no new files were seen (perhaps don't even open it in write mode?)
        ++m_ExistingSigs;
        ++m_TotalSigs;
    }
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormat::recursivelyGpgSignDirEntriesAndEmitFinishedWhenNoMore() //pseudo-recursive (async) -- head
{
    while(m_DirIterator->hasNext())
    {
        m_DirIterator->next();
        const QFileInfo &currentEntry = m_DirIterator->fileInfo();
        if(currentEntry.isFile())
        {
            const QString &relativeFilePath = currentEntry.absoluteFilePath().mid(m_CharacterLengthOfAbsolutePathOfTargetDir_IncludingTrailingSlash);
            if(m_ExcludeEntries.contains(relativeFilePath))
                continue;
            m_FileMetaCurrentlyBeingGpgSigned.FilePath = relativeFilePath;
            m_FileMetaCurrentlyBeingGpgSigned.UnixTimestampInSeconds = (currentEntry.lastModified().toMSecsSinceEpoch() / 1000);
            QHash<QString /*file path*/, RecursiveCustomDetachedSignaturesFileMeta /*file meta*/>::iterator it = m_AllSigsFromSigFileSoWeKnowWhichOnesToSkipAsWeRecurseTheFilesystem.find(relativeFilePath);
            if(it != m_AllSigsFromSigFileSoWeKnowWhichOnesToSkipAsWeRecurseTheFilesystem.end())
            {
                //currentEntry is already in sigfile

                //early warning detection of accidental overwrites: did the last modified timestamp change?
                if(!m_RecursiveCustomDetachedSignatures->filesystemLastModifiedUnixTimestampAndMetaUnixTimestampsAreIdentical(m_FileMetaCurrentlyBeingGpgSigned.UnixTimestampInSeconds, it.value()))
                {
                    emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(false);
                    return;
                }

                //last modified timestamp did not change

                //move it to the map for outputting to the new sigs file
                m_AllSigsForOutputting.insert(it.key(), it.value());
                m_AllSigsFromSigFileSoWeKnowWhichOnesToSkipAsWeRecurseTheFilesystem.erase(it);
            }
            else
            {
                //currentEntry is not in sigfile
                gpgSignFileAndThenContinueRecursingDir();
                ++m_AddedSigs;
                ++m_TotalSigs;
                return;
            }
        }
    }

    //detect files/sigs in sig file that do not exist on filesystem -- TODOoptional: --ignore-existing-sigs-with-missing-file (and perhaps it can be specified on a file by file basis... or just for every file encountered for this run)
    if(!m_AllSigsFromSigFileSoWeKnowWhichOnesToSkipAsWeRecurseTheFilesystem.isEmpty())
    {
        emit e("the below files in the gpg signatures file were not found on the filesystem:");
        emit e("");
        QHashIterator<QString /*file path*/, RecursiveCustomDetachedSignaturesFileMeta /*file meta*/> it(m_AllSigsFromSigFileSoWeKnowWhichOnesToSkipAsWeRecurseTheFilesystem);
        while(it.hasNext())
        {
            it.next();
            emit e(it.key());
        }
        emit e("");
        emit e("the above files in the gpg signatures file were not found on the filesystem");
        emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(false);
        return;
    }

    if(m_AddedSigs > 0)
    {
        //now write the final custom detached signatures file
        if(m_InputAndOutputSigFileTextStream.device()->isOpen()) //not open == doesn't exist
            m_InputAndOutputSigFileTextStream.device()->close(); //we had it opened in read only mode
        if(!m_InputAndOutputSigFileTextStream.device()->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        {
            emit e("failed to re-open signatures file for writing"); //TODOlol: how to work on iodevices and still know the filename?
            emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(false);
            return;
        }
        QMapIterator<QString /*file path*/, RecursiveCustomDetachedSignaturesFileMeta /*file meta*/> it(m_AllSigsForOutputting);
        while(it.hasNext())
        {
            it.next();
            m_InputAndOutputSigFileTextStream << it.value();
        }
        m_InputAndOutputSigFileTextStream.flush();
    }

    emit o("(sigs, existing: " + QString::number(m_ExistingSigs) + ", added: " + QString::number(m_AddedSigs) + ", total: " + QString::number(m_TotalSigs) + ")");
    emit o("done recusrively signing directory -- everything OK" + ((m_AddedSigs > 0) ? QString() : QString(" (nothing added)")));
    emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(true);
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormat::gpgSignFileAndThenContinueRecursingDir()
{
    QStringList gpgArgs;
    gpgArgs << "--detach-sign" << "--armor" << "-o" << "-" << m_FileMetaCurrentlyBeingGpgSigned.FilePath;
    m_GpgProcess->start(GPG_DEFAULT_PATH, gpgArgs, QIODevice::ReadOnly);
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormat::spitOutGpgProcessOutput()
{
    emit e(m_GpgProcess->readAllStandardError());
    emit e(m_GpgProcess->readAllStandardOutput());
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormat::recursivelyGpgSignIntoCustomDetachedSignaturesFormat(const QString &dirToRecursivelySign, const QString &outputSigFilePath, bool forceResigningOfFilesAlreadyPresentInOutputSigFile, const QStringList &excludeEntries)
{
    QFile *outputSigFile = new QFile(outputSigFilePath, this);
    QFileInfo outputSigFileInfo(*outputSigFile);
    const QString &absolutePathOfDirToRecursivelySign_WithSlashAppended = appendSlashIfNeeded(dirToRecursivelySign);
    const QString &outputSigsFileAbsolutePath = outputSigFileInfo.absoluteFilePath();
    QStringList excludeEntriesWithOutputSigsFilePossiblyAddedToExclusionList = excludeEntries; //cow
    if(outputSigsFileAbsolutePath.startsWith(absolutePathOfDirToRecursivelySign_WithSlashAppended))
    {
        //output sigs file is in target dir, so exclude it
        QString outputSigsFileRelativePath = outputSigsFileAbsolutePath.mid(absolutePathOfDirToRecursivelySign_WithSlashAppended.length());
        excludeEntriesWithOutputSigsFilePossiblyAddedToExclusionList << outputSigsFileRelativePath;
    }
    if(outputSigFileInfo.exists())
    {
        if(!outputSigFileInfo.isFile())
        {
            emit e("sigsfile exists but is not a file");
            emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(false);
            return;
        }
        if(!outputSigFile->open(QIODevice::ReadOnly | QIODevice::Text))
        {
            emit e("failed to open sig file for reading: " + outputSigFilePath);
            emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(false);
            return;
        }
    }
    recursivelyGpgSignIntoCustomDetachedSignaturesFormat(dirToRecursivelySign, outputSigFile, forceResigningOfFilesAlreadyPresentInOutputSigFile, excludeEntriesWithOutputSigsFilePossiblyAddedToExclusionList);
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormat::recursivelyGpgSignIntoCustomDetachedSignaturesFormat(const QDir &dirToRecursivelySign, QIODevice *outputSigIoDevice, bool forceResigningOfFilesAlreadyPresentInOutputSigFile, const QStringList &excludeEntries) //TODOoptional: --force-resign flag, but non-default. TODOoptional: better than a simple "force-resign" flag, we could store the last modified timestamp on the file path line... and as we iterate, we compare modified timestamps. if they don't match, we re-verify. if the sigs don't match, we notify (and i guess the user could opt to overwriting with the new sig (depends on use case entirely)). if the sig does verify, we 'touch' the file so that it gets it's old timestamp back (alternatively but less likely, we update the last modified timestamp in the sigfile to the one seen on the filesystem... it could be a "whichever is earlier" algorithm... but I think in general I'll want to 'touch' the fs timestamp with the one found in the sigfile)
{
    if(!dirToRecursivelySign.exists())
    {
        emit e("dir does not exist: " + dirToRecursivelySign.absolutePath());
        emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(false);
        return;
    }
    m_InputAndOutputSigFileTextStream.setDevice(outputSigIoDevice);
    readInAllSigsFromSigFileSoWeKnowWhichOnesToSkipAsWeRecurseTheFilesystem();
    const QString &absolutePathOfDirToRecursivelySign = dirToRecursivelySign.absolutePath();
    m_GpgProcess->setWorkingDirectory(absolutePathOfDirToRecursivelySign);
    m_CharacterLengthOfAbsolutePathOfTargetDir_IncludingTrailingSlash = absolutePathOfDirToRecursivelySign.length() + 1; //+1 to account for trailing slash
    m_DirIterator.reset(new QDirIterator(absolutePathOfDirToRecursivelySign, (QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden), QDirIterator::Subdirectories));
    m_ExcludeEntries = excludeEntries;
    recursivelyGpgSignDirEntriesAndEmitFinishedWhenNoMore(); //pseudo-recursive (async) -- first head call
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormat::handleGpgProcessError(QProcess::ProcessError processError)
{
    emit e("gpg qprocess error: '" + QString::number(processError) + "' while trying to sign file: " + m_FileMetaCurrentlyBeingGpgSigned.FilePath);
    emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(false);
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormat::handleGpgProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus != QProcess::NormalExit)
    {
        spitOutGpgProcessOutput();
        emit e("gpg did not exit normally when trying to sign file: " + m_FileMetaCurrentlyBeingGpgSigned.FilePath);
        emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(false);
        return;
    }
    if(exitCode != 0)
    {
        spitOutGpgProcessOutput();
        emit e("gpg did not exit with code 0. gpg exitted with code: " + QString::number(exitCode) + " while trying to sign file: " + m_FileMetaCurrentlyBeingGpgSigned.FilePath);
        emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(false);
        return;
    }

    m_FileMetaCurrentlyBeingGpgSigned.GpgSignature = m_GpgProcessTextStream.readAll();

    //TODOoptional: if(verbose) { file <name> signed } -- random/semi-OT: instead of littering your code with if(m_Quiet), if(m_Verbose), etc... just do emit e/o/v as usual and then [dis-]connect the signals accordingly. this marks the first time i've ever thought of using "emit v"

    m_AllSigsForOutputting.insert(m_FileMetaCurrentlyBeingGpgSigned.FilePath, m_FileMetaCurrentlyBeingGpgSigned);
    recursivelyGpgSignDirEntriesAndEmitFinishedWhenNoMore(); //pseudo-recursive (async) -- tail
}
