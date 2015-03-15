#include "recursivelygpgsignintocustomdetachedsignaturesformat.h"

#include <QFile>
#include <QDir>
#include <QHashIterator>
#include <QMapIterator>

#include "recursivecustomdetachedgpgsignatures.h"

//TODOoptional: a combination of sign+verify: sign files on fs not in sigfile, verify files in sigfile, report sigs in sigfile that do not exist on filesystem -- it shouldn't be the default because reading/verifying EVERY file would take a long as fuck time
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
    while(!m_OutputSigFileTextStream.atEnd())
    {
        QString alreadySignedFilePath;
        QString alreadySignedFileSig;
        if(!m_RecursiveCustomDetachedSignatures->readPathAndSignature(m_OutputSigFileTextStream, &alreadySignedFilePath, &alreadySignedFileSig))
        {
            emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(false);
            return;
        }
        m_AllSigsFromSigFileSoWeKnowWhichOnesToSkipAsWeRecurseTheFilesystem.insert(alreadySignedFilePath, alreadySignedFileSig); //TODOreq: we start off with a hash, but as their existences are verified, we move them into a map... the same map that new files+sigs are being placed into. we want it to be sorted for when we re-write the signature file with the new entries. TODOoptimization: don't re-write the sigs file if no new files were seen (perhaps don't even open it in write mode?)
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
            const QString &relativeFilePath = currentEntry.absoluteFilePath().mid(m_CharacterLengthOfAbsolutePathOfTargetDir_IncludingTrailingSlash); //TODOmb: colon and last modified timestamp? maybe I shouldn't mix the two... but eh maybe I should? sign time != modify time, after all...
            QHash<QString, QString>::iterator it = m_AllSigsFromSigFileSoWeKnowWhichOnesToSkipAsWeRecurseTheFilesystem.find(relativeFilePath);
            if(it != m_AllSigsFromSigFileSoWeKnowWhichOnesToSkipAsWeRecurseTheFilesystem.end())
            {
                //currentEntry is already in sigfile

                //move it to the map
                m_AllSigsForOutputting.insert(it.key(), it.value());
                m_AllSigsFromSigFileSoWeKnowWhichOnesToSkipAsWeRecurseTheFilesystem.erase(it);
            }
            else
            {
                //currentEntry is not in sigfile
                gpgSignFileAndThenContinueRecursingDir(relativeFilePath);
                ++m_AddedSigs;
                ++m_TotalSigs;
                return;
            }
        }
    }

    //detect files/sigs in sig file that do not exist on filesystem
    if(!m_AllSigsFromSigFileSoWeKnowWhichOnesToSkipAsWeRecurseTheFilesystem.isEmpty())
    {
        emit e("the below files in the gpg signatures file were not on the filesystem:");
        emit e("");
        QHashIterator<QString /*file path*/, QString /*file sig*/> it(m_AllSigsFromSigFileSoWeKnowWhichOnesToSkipAsWeRecurseTheFilesystem);
        while(it.hasNext())
        {
            it.next();
            emit e(it.key());
        }
        emit e("");
        emit e("the above files in the gpg signatures file were not on the filesystem");
        emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(false);
        return;
    }

    //now write the final custom detached signatures file
    m_OutputSigFileTextStream.device()->close();
    if(!m_OutputSigFileTextStream.device()->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        emit e("failed to re-open signatures file for writing"); //TODOlol: how to work on iodevices and still know the filename?
        emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(false);
        return;
    }
    QMapIterator<QString /*file path*/, QString /*file sig*/> it(m_AllSigsForOutputting);
    while(it.hasNext())
    {
        m_OutputSigFileTextStream << it.key() << endl << it.value() << endl;
    }
    m_OutputSigFileTextStream.flush();

    emit o("(sigs, existing: " + QString::number(m_ExistingSigs) + ", added: " + QString::number(m_AddedSigs) + ", total: " + QString::number(m_TotalSigs));
    emit o("done recusrively signing directory -- everything OK");
    emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(true);
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormat::gpgSignFileAndThenContinueRecursingDir(const QString &filePathToGpgSign)
{
    m_FilePathCurrentlyBeingGpgSigned = filePathToGpgSign;

    QStringList gpgArgs;
    gpgArgs << "--detach-sign" << "--armor" << "-o" << "-" << filePathToGpgSign;
    m_GpgProcess->start(GPG_DEFAULT_PATH, gpgArgs, QIODevice::ReadOnly);
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormat::spitOutGpgProcessOutput()
{
    emit e(m_GpgProcess->readAllStandardError());
    emit e(m_GpgProcess->readAllStandardOutput());
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormat::recursivelyGpgSignIntoCustomDetachedSignaturesFormat(const QString &dirToRecursivelySign, const QString &outputSigFilePath, bool forceResigningOfFilesAlreadyPresentInOutputSigFile)
{
    QFile *outputSigFile = new QFile(outputSigFilePath);
    if(!outputSigFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit e("failed to open sig file for reading and writing: " + outputSigFilePath);
        emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(false);
        return;
    }
    recursivelyGpgSignIntoCustomDetachedSignaturesFormat(dirToRecursivelySign, outputSigFile, forceResigningOfFilesAlreadyPresentInOutputSigFile);
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormat::recursivelyGpgSignIntoCustomDetachedSignaturesFormat(const QDir &dirToRecursivelySign, QIODevice *outputSigIoDevice, bool forceResigningOfFilesAlreadyPresentInOutputSigFile) //TODOoptional: --force-resign flag, but non-default. TODOoptional: better than a simple "force-resign" flag, we could store the last modified timestamp on the file path line... and as we iterate, we compare modified timestamps. if they don't match, we re-verify. if the sigs don't match, we notify (and i guess the user could opt to overwriting with the new sig (depends on use case entirely)). if the sig does verify, we 'touch' the file so that it gets it's old timestamp back (alternatively but less likely, we update the last modified timestamp in the sigfile to the one seen on the filesystem... it could be a "whichever is earlier" algorithm... but I think in general I'll want to 'touch' the fs timestamp with the one found in the sigfile)
{
    if(!dirToRecursivelySign.exists())
    {
        emit e("dir does not exist: " + dirToRecursivelySign.absolutePath());
        emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(false);
        return;
    }
    m_OutputSigFileTextStream.setDevice(outputSigIoDevice);
    readInAllSigsFromSigFileSoWeKnowWhichOnesToSkipAsWeRecurseTheFilesystem();
    const QString &absolutePathOfDirToRecursivelySign = dirToRecursivelySign.absolutePath();
    m_CharacterLengthOfAbsolutePathOfTargetDir_IncludingTrailingSlash = absolutePathOfDirToRecursivelySign.length() + 1; //+1 to account for trailing slash
    m_DirIterator.reset(new QDirIterator(absolutePathOfDirToRecursivelySign, (QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden), QDirIterator::Subdirectories));
    recursivelyGpgSignDirEntriesAndEmitFinishedWhenNoMore(); //pseudo-recursive (async) -- first head call
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormat::handleGpgProcessError(QProcess::ProcessError processError)
{
    emit e("gpg qprocess error: '" + QString::number(processError) + "' while trying to sign file: " + m_FilePathCurrentlyBeingGpgSigned);
    emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(false);
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormat::handleGpgProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus != QProcess::NormalExit)
    {
        spitOutGpgProcessOutput();
        emit e("gpg did not exit normally when trying to sign file: " + m_FilePathCurrentlyBeingGpgSigned);
        emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(false);
        return;
    }
    if(exitCode != 0)
    {
        spitOutGpgProcessOutput();
        emit e("gpg did not exit with code 0. gpg exitted with code: " + QString::number(exitCode) + " while trying to sign file: " + m_FilePathCurrentlyBeingGpgSigned);
        emit doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(false);
        return;
    }

    QString fileSig = m_GpgProcessTextStream.readAll();

    //TODOoptional: if(verbose) { file <name> signed }

    m_AllSigsForOutputting.insert(m_FilePathCurrentlyBeingGpgSigned, fileSig);
    recursivelyGpgSignDirEntriesAndEmitFinishedWhenNoMore(); //pseudo-recursive (async) -- tail
}
