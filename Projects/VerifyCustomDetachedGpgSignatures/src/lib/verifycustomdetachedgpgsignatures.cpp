#include "verifycustomdetachedgpgsignatures.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QProcess>

#define GPG_END_SIG_DELIMITER "-----END PGP SIGNATURE-----"
#define GPG_DEFAULT_PATH "/usr/bin/gpg" //TODOoptional: custom path as arg

VerifyCustomDetachedGpgSignatures::VerifyCustomDetachedGpgSignatures(QObject *parent)
    : QObject(parent)
    , m_GpgProcess(new QProcess(this))
    , m_GpgProcessTextStream(m_GpgProcess)
{
    m_GpgProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_GpgProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(handleGpgProcessError(QProcess::ProcessError)));
    connect(m_GpgProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(handleGpgProcessFinished(int,QProcess::ExitStatus)));
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

#if 0 //fail (anti-pattern)
        QString nextLine = m_CustomDetachedSignaturesStream.readLine();
        if(!nextLine.isEmpty())
        {
            QString currentFilePathToVerify;
            QString currentFileSignature;
            if(readPathAndSignature(nextLine, &currentFilePathToVerify, &currentFileSignature))
            {
                verifyFileSignatureAndThenContinueOntoNextEntryOfCustomDetached(currentFilePathToVerify, currentFileSignature);
                return;
            }
            else
            {
                emit e("failed to readPathAndSignature"); //TODOoptional: offending line number
                emit doneVerifyingCustomDetachedGpgSignatures(false);
                return;
            }
        }
#endif
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
    QStringList gpgArgs;
    gpgArgs << "--verify" << "-" << filePathToVerify;
    m_GpgProcess->start(GPG_DEFAULT_PATH, gpgArgs, QIODevice::ReadWrite);
    m_GpgProcessTextStream << fileSignature;
    m_GpgProcessTextStream.flush(); //necessary? mb not, but fuck it
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
    m_CustomDetachedSignaturesStream.setDevice(customDetachedSignaturesIoDevice);
    verifyNextEntryOfCustomDetachedOrEmitFinishedIfNoMore(); //pseudo-recursive (async) -- first head call
}
void VerifyCustomDetachedGpgSignatures::handleGpgProcessError(QProcess::ProcessError processError)
{
    emit e("gpg process error: " + processError);
    emit doneVerifyingCustomDetachedGpgSignatures(false); //TODOoptional: this and handleGpgProcessFinished might both emit this signal. probably doesn't matter
}
void VerifyCustomDetachedGpgSignatures::handleGpgProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus != QProcess::NormalExit)
    {
        spitOutGpgProcessOutput();
        emit e("gpg did not exit normally");
        emit doneVerifyingCustomDetachedGpgSignatures(false);
        return;
    }
    if(exitCode != 0)
    {
        spitOutGpgProcessOutput();
        emit e("one of the signatures was not valid, or there was some other reason gpg did not exit with code 0. gpg exitted with code: " + QString::number(exitCode)); //TODOoptional: this, and other errors, can/should show the current file (and possibly sig) that failed to verify
        emit doneVerifyingCustomDetachedGpgSignatures(false);
        return;
    }

    //read + ignore the gpg output
    m_GpgProcessTextStream.readAll();

    //TODOoptional: if(verbose) { file <name> verified }

    verifyNextEntryOfCustomDetachedOrEmitFinishedIfNoMore(); //pseudo-recursive (async) -- tail
}
