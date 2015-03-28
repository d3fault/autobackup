#include "watchsigsfileandpostchangestousenet.h"

#include <QMutableHashIterator>
#include <QCryptographicHash>
#include <QMimeDatabase>
#include <QProcess>

#define WatchSigsFileAndPostChangesToUsenet_FILE_BIN "file"
#define WatchSigsFileAndPostChangesToUsenet_POSTNEWS_BIN "postnews"
#define EEEEEEEE_WatchSigsFileAndPostChangesToUsenet(msg) { emit e(msg); emit doneWatchingSigsFileAndPostingChangesToUsenet(false); return; }

WatchSigsFileAndPostChangesToUsenet::WatchSigsFileAndPostChangesToUsenet(QObject *parent)
    : QObject(parent)
    , m_SigsFileWatcher(new QFileSystemWatcher(this)) //the best reason for pointers: qobject parenting (which makes moveToThread work). If it's not a pointer (a stack member) and sets 'this' as parent, then it gets double deleted xD
    , m_RecursiveCustomDetachedSignatures(new RecursiveCustomDetachedSignatures(this))
    , m_PostnewsProcess(new QProcess(this))

{
    m_PostnewsProcess->setProcessChannelMode(QProcess::ForwardedChannels);

    connect(m_SigsFileWatcher, SIGNAL(fileChanged(QString)), this, SLOT(handleSigsFileChanged(QString)));
    connect(m_RecursiveCustomDetachedSignatures, SIGNAL(e(QString)), this, SIGNAL(e(QString)));
    connect(m_PostnewsProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(handlePostnewsProcessFinished(int,QProcess::ExitStatus)));
}
bool WatchSigsFileAndPostChangesToUsenet::startWatchingSigsFile(const QString &sigsFilePath)
{
    if(!m_SigsFileWatcher->addPath(sigsFilePath))
    {
        emit e("failed to start watching sigsfile: " + sigsFilePath);
        emit doneWatchingSigsFileAndPostingChangesToUsenet(false);
        return false;
    }
    return true;
}
void WatchSigsFileAndPostChangesToUsenet::readInSigsFileAndPostAllNewEntries(const QString &sigsFilePath)
{
    QFile sigsFileToWatch(sigsFilePath);
    if(!sigsFileToWatch.open(QIODevice::ReadOnly | QIODevice::Text))
        EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("failed to open sigs file for reading: " + sigsFilePath)
    QHash<QString /*file path*/, RecursiveCustomDetachedSignaturesFileMeta /*file meta*/> sigsFromSigFile;
    if(!m_RecursiveCustomDetachedSignatures->readInAllSigsFromSigFile(&sigsFileToWatch, &sigsFromSigFile))
        EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("failed to read in all sigs from file:" + sigsFilePath);
    QSet<RecursiveCustomDetachedSignaturesFileMeta> sigsFromFileAsSet = sigsFromSigFile.values().toSet();
    sigsFromFileAsSet.subtract(m_FilesAlreadyPostedOnUsenet); //TODOreq: populate. I either have to keep the entire fs hierarchy+sigs in memory (only a few mb at the moment, but it will grow!!!), or I have to read them from disk every time the sigsfile being watched changes... idfk. There are clever ways to solve that problem ofc...
    //QHash<QString /*file path*/, RecursiveCustomDetachedSignaturesFileMeta /*file meta*/> sigsAlreadyPostedOrEnqueuedForPosting;
    /*QMutableHashIterator<QString, RecursiveCustomDetachedSignaturesFileMeta> newSigsIterator(sigsFromSigFile);
    while(newSigsIterator.hasNext())
    {
        newSigsIterator.next();
        if(sigsAlreadyPostedOrEnqueuedForPosting.contains(newSigsIterator.key()))
            newSigsIterator.remove(); //already enqueued for posting or posted, so don't double enqueue/post
    }*/

    //at this point, sigsFromSigFile only contains new entries. so enqueue them
    m_FilesEnqueuedForPostingToUsenet.unite(sigsFromFileAsSet); //arbitrary posting order (QSet), fuck it. too lazy to sort by timestamp
    postAnEnqueuedFileIfNotAlreadyPostingOne();
}
void WatchSigsFileAndPostChangesToUsenet::postAnEnqueuedFileIfNotAlreadyPostingOne()
{
    if(!m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet.isNull())
        return;
    if(m_FilesEnqueuedForPostingToUsenet.isEmpty())
        return;
    const RecursiveCustomDetachedSignaturesFileMeta &nextFile = *(m_FilesEnqueuedForPostingToUsenet.begin());
    m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet.reset(new RecursiveCustomDetachedSignaturesFileMeta(nextFile));
    postToUsenet(nextFile);
}
void WatchSigsFileAndPostChangesToUsenet::postToUsenet(const RecursiveCustomDetachedSignaturesFileMeta &nextFile)
{
    QFile file(nextFile.FilePath);
    if(!file.open(QIODevice::ReadOnly))
        EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("failed to open file: " + nextFile.FilePath)
    const QByteArray &fileContents = file.readAll();
    const QByteArray &fileContentsBase64 = fileContents.toBase64(); //TODOreq: wrap at 72 chars
    //TODOmb: const QByteArray &fileContentsSha1Base64 =

    QByteArray body = wrap(fileContentsBase64, 72);
    const QByteArray &fileContentsMd5 = QCryptographicHash::hash(fileContents, QCryptographicHash::Sha1);
    QByteArray contentMd5Base64 = fileContentsMd5.toBase64();
    QByteArray boundary = "-_"; //Underscore also because pgp sigs would accidentally have a boundary in them. //fileContentsMd5.toHex(); //heh

    QFileInfo fileInfo(file);
    QByteArray fileNameOnly = fileInfo.fileName().toLatin1();
    QByteArray mime = m_MimeDatabase.mimeTypeForFile(fileInfo).name().toLatin1();
    //if(!getMimeFromFileProcess(nextFile.FilePath, &mime))
    //    return;
    QByteArray post("From: d3fault@d3fault.net\r\nNewsgroups: alt.binaries.boneless\r\nOrganization: d3fault\r\nMime-Version 1.0\r\nSubject: d3fault.net/binary/" + nextFile.FilePath.toLatin1() + "\r\nContent-Type: multipart/signed; boundary=" + boundary + "; protocol=\"application/pgp-signature\"\r\n\r\n--" + boundary + "\r\nContent-Type: " + mime + "; name=\"" + fileNameOnly + "\"\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: inline; filename=\"" + fileNameOnly + "\"\r\nContent-MD5: " + contentMd5Base64 + "\r\n\r\n" + body + "\r\n\r\n--" + boundary + "\r\nContent-Type: application/pgp-signature\r\n\r\n" + nextFile.GpgSignature.toLatin1() + "\r\n\r\n--" + boundary + "\r\n"); //TODOoptional: micalg=???. TODOoptional: customizeable parts of this  from command line ofc lol. TODOmb: Message-ID (even if it's not an sha1 or whatever, it'd still be good to know it so we don't have to query it. We could start with sha1 and then keep trying different values if it doesn't work)
    emit o("Trying to post: " + nextFile.FilePath);

    QStringList postnewsArgs;
    QString m_UsenetAuthUsername;
    QString m_UsenetAuthPassword;
    QString m_PortString;
    QString m_UsenetServer;
    postnewsArgs << "--verbose" << ("--user=" + m_UsenetAuthUsername) << ("--pass=" + m_UsenetAuthPassword) << ("--port=" + m_PortString) << m_UsenetServer;
    m_PostnewsProcess->start(WatchSigsFileAndPostChangesToUsenet_POSTNEWS_BIN, postnewsArgs);
    if(!m_PostnewsProcess->write(post))
        EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("failed to write post to postnews. filepath: " + nextFile.FilePath);
    m_PostnewsProcess->closeWriteChannel();
}
QByteArray WatchSigsFileAndPostChangesToUsenet::wrap(const QString &toWrap, int wrapAt) //zzz
{
    QByteArray ret;
    int toWrapLen = toWrap.length();
    ret.reserve(toWrapLen); //minimum
    int currentColumn = 1;
    for(int i = 0; i < toWrapLen; ++i)
    {
        ret.append(toWrap.at(i));
        ++i;
        if((++currentColumn) == wrapAt)
        {
            ret.append("\r\n");
            currentColumn = 1; //might overflow! oh wait so will i ;-P
        }
    }
    return ret;
}
#if 0
bool WatchSigsFileAndPostChangesToUsenet::getMimeFromFileProcess(const QString &filePath, QByteArray *out_Mime)
{
    QProcess fileProcess;
    fileProcess.setProcessChannelMode(QProcess::ForwardedErrorChannel);
    QStringList fileArgs;
    fileArgs << "--mime" << "--brief" << filePath;
    fileProcess.start(WatchSigsFileAndPostChangesToUsenet_FILE_BIN, fileArgs, QIODevice::ReadOnly);
    if(!fileProcess.waitForStarted(-1))
    {
        emit e("file process failed to start: " + fileProcess.readAllStandardOutput());
        emit doneWatchingSigsFileAndPostingChangesToUsenet(false);
        return false;
    }
    if(!fileProcess.waitForFinished(-1))
    {
        emit e("file process failed to finish: " + fileProcess.readAllStandardOutput());
        emit doneWatchingSigsFileAndPostingChangesToUsenet(false);
        return false;
    }
    if(fileProcess.exitCode() != 0 || fileProcess.exitStatus() != QProcess::NormalExit)
    {
        emit e("file process exitted abnormally with exit code: " + fileProcess.exitCode() + " -- " + fileProcess.readAllStandardOutput());
        emit doneWatchingSigsFileAndPostingChangesToUsenet(false);
        return false;
    }
    out_Mime->append(fileProcess.readAll().trimmed());
    return true;
}
#endif
void WatchSigsFileAndPostChangesToUsenet::startWatchingSigsFileAndPostChangesToUsenet(const QString &sigsFilePathToWatch, const QString &dirCorrespondingToSigsFile, const QString &dataDirForKeepingTrackOfAlreadyPostedFiles)
{
    QDir dirCorrespondingToSigsFileInstance(dirCorrespondingToSigsFile);
    QDir dataDirForKeepingTrackOfAlreadyPostedFilesInstance(dataDirForKeepingTrackOfAlreadyPostedFiles);
    startWatchingSigsFileAndPostChangesToUsenet(sigsFilePathToWatch, dirCorrespondingToSigsFileInstance, dataDirForKeepingTrackOfAlreadyPostedFilesInstance);
}
void WatchSigsFileAndPostChangesToUsenet::startWatchingSigsFileAndPostChangesToUsenet(const QString &sigsFilePathToWatch /*this was a QIODevice, but QFsw can't watch an iodevice (readyRead() misleads an amateur :-P)*/, const QDir &dirCorrespondingToSigsFile, const QDir &dataDirForKeepingTrackOfAlreadyPostedFiles)
{
    m_SigsFileWatcher->removePaths(m_SigsFileWatcher->files());
    if(!startWatchingSigsFile(sigsFilePathToWatch))
        return;
    if(!dirCorrespondingToSigsFile.exists())
        EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("dir corresponding to sigsfile does not exist:" + dirCorrespondingToSigsFile.absolutePath())
    if(!dataDirForKeepingTrackOfAlreadyPostedFiles.exists())
        EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("data dir does not exist:" + dataDirForKeepingTrackOfAlreadyPostedFiles.absolutePath())

                readInSigsFileAndPostAllNewEntries(sigsFilePathToWatch); //start posting right away
}
void WatchSigsFileAndPostChangesToUsenet::quitCleanly()
{
    //TODOreq
}
void WatchSigsFileAndPostChangesToUsenet::handleSigsFileChanged(const QString &sigsFilePath)
{
    if(!startWatchingSigsFile(sigsFilePath))
        return;
    readInSigsFileAndPostAllNewEntries(sigsFilePath);
}
void WatchSigsFileAndPostChangesToUsenet::handlePostnewsProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitCode != 0 || exitStatus != QProcess::NormalExit)
        EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("postnews exitted abnormally with exit code: " + QString::number(exitCode))
    RecursiveCustomDetachedSignaturesFileMeta postedFile(*m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet.data());
    emit o("file posted to usenet: " + postedFile.FilePath);
    m_FilesAlreadyPostedOnUsenet.insert(postedFile); //TODOreq: serialize this at app shutdown, load it (if it exists) at app start
    m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet.reset();
    postAnEnqueuedFileIfNotAlreadyPostingOne();
}
