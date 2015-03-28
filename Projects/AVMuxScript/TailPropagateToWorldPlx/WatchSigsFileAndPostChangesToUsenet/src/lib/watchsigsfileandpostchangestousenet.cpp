#include "watchsigsfileandpostchangestousenet.h"

#include <QMutableHashIterator>
#include <QCryptographicHash>
#include <QMimeDatabase>
#include <QProcess>
#include <QStandardPaths>
#include <QSaveFile>
#include <QSetIterator>

//#define WatchSigsFileAndPostChangesToUsenet_FILE_BIN "file"
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
bool WatchSigsFileAndPostChangesToUsenet::readInAlreadyPostedFilesSoWeDontDoublePost()
{
    if(!QFile::exists(m_AlreadyPostedFilesAllSigsIshFilePath))
        return true; //first run, doesn't exist
    QFile alreadyPostedFilesFile(m_AlreadyPostedFilesAllSigsIshFilePath);
    if(!alreadyPostedFilesFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //if open for reading fails, open for writing probably will too. or that could mean that next time this app is launched we can't read this file, so blah error out quick
        emit e("failed to open for reading the list of already posted files... file: " + m_AlreadyPostedFilesAllSigsIshFilePath);
        emit doneWatchingSigsFileAndPostingChangesToUsenet(false);
        return false;
    }
    QHash<QString /*file path*/, RecursiveCustomDetachedSignaturesFileMeta /*file meta*/> alreadyPostedFiles;
    if(!m_RecursiveCustomDetachedSignatures->readInAllSigsFromSigFile(&alreadyPostedFilesFile, &alreadyPostedFiles))
    {
        emit e("failed to read in list of already posted files... file: " + m_AlreadyPostedFilesAllSigsIshFilePath);
        emit doneWatchingSigsFileAndPostingChangesToUsenet(false);
        return false;
    }
    m_FilesAlreadyPostedOnUsenet = alreadyPostedFiles.values().toSet();
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
    sigsFromFileAsSet.subtract(m_FilesAlreadyPostedOnUsenet); //TODOoptimization: I either have to keep the entire fs hierarchy+sigs in memory (only a few mb at the moment, but it will grow!!!), or I have to read them from disk every time the sigsfile being watched changes... idfk. There are clever ways to solve that problem ofc (and it's worth noting we don't NEED the sigs of the already posted files in memory, only need the file paths (this alone is probably ~80% of that memory now freed xD))...
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
    postAnEnqueuedFileIfNotAlreadyPostingOne_OrQuitIfCleanQuitRequested();
}
void WatchSigsFileAndPostChangesToUsenet::postAnEnqueuedFileIfNotAlreadyPostingOne_OrQuitIfCleanQuitRequested()
{
    if(!m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet.isNull())
        return;
    if(m_CleanQuitRequested)
    {
        if(rememberFilesAlreadyPostedOnUsenetSoWeKnowWhereToResumeNextTime()) //if it fails, it emits doneWatchingSigsFileAndPostingChangesToUsenet(false); -- i originally was passing this method as the arg of doneWatchingSigsFileAndPostingChangesToUsenet... but i remembered that I want to call rememberFilesAlreadyPostedOnUsenetSoWeKnowWhereToResumeNextTime whenever the post queue becomes empty TODOreq (because the system could crash at any moment (or more likely, i forget to cleanly shut this app down before a reboot) :-P)
            emit doneWatchingSigsFileAndPostingChangesToUsenet(true);
        return;
    }
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

    QByteArray body;
    QByteArray fileContentsMd5;

    {
        const QByteArray &fileContents = file.readAll(); //1x full copy in memory
        const QByteArray &fileContentsBase64 = fileContents.toBase64(); //2x full copies in memory
        body = wrap(fileContentsBase64, 72); //3x full copies in memory
        fileContentsMd5 = QCryptographicHash::hash(fileContents, QCryptographicHash::Sha1);
    }//2 of the full copies in memory go out of scope <3

    QByteArray contentMd5Base64 = fileContentsMd5.toBase64();
    QByteArray boundary = "-whatever_the-laws_of-physics_allow-"; //Underscores are necesary because gpg sigs would accidentally have a boundary in them. //fileContentsMd5.toHex(); //heh

    QFileInfo fileInfo(file);
    QByteArray fileNameOnly = fileInfo.fileName().toLatin1();
    QByteArray mime = m_MimeDatabase.mimeTypeForFile(fileInfo).name().toLatin1();
    //if(!getMimeFromFileProcess(nextFile.FilePath, &mime))
    //    return;

    //TODOreq: file splitting ('hat' will make large videos every day, and there's also the relics to consider)
    //TODOreq: message-id (dep file splitting)

    QByteArray post("From: d3fault@d3fault.net\r\nNewsgroups: alt.binaries.boneless\r\nOrganization: d3fault\r\nMime-Version 1.0\r\nSubject: d3fault.net/binary/" + nextFile.FilePath.toLatin1() + "\r\nContent-Type: multipart/signed; boundary=" + boundary + "; protocol=\"application/pgp-signature\"\r\n\r\n--" + boundary + "\r\nContent-Type: " + mime + "; name=\"" + fileNameOnly + "\"\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: inline; filename=\"" + fileNameOnly + "\"\r\nContent-MD5: " + contentMd5Base64 + "\r\n\r\n" + body + "\r\n\r\n--" + boundary + "\r\nContent-Type: application/pgp-signature\r\nContent-Disposition: attachment; filename=\"" + fileNameOnly + ".asc\"\r\n\r\n" + nextFile.GpgSignature.toLatin1() + "\r\n\r\n--" + boundary + "\r\n"); //TODOoptional: micalg=???. TODOoptional: customizeable parts of this  from command line ofc lol. TODOmb: Message-ID (even if it's not an sha1 or whatever, it'd still be good to know it so we don't have to query it. We could start with sha1 and then keep trying different values if it doesn't work)
    emit o("Trying to post: " + nextFile.FilePath);

    QStringList postnewsArgs;
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
bool WatchSigsFileAndPostChangesToUsenet::rememberFilesAlreadyPostedOnUsenetSoWeKnowWhereToResumeNextTime()
{
    //TODOoptimization: noop/return-true if no NEW files have been posted
    //TODOoptional: when recursive gpg signer makes the file, it's sorted by the filepath. I have no need to sort it like that (but could easily), so I won't (THINK OF THE CLOCK CYCLES!!)
    QSaveFile saveFile(m_AlreadyPostedFilesAllSigsIshFilePath);
    if(!saveFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit e("failed to open for writing the list of already posted files to disk for next time: " + m_AlreadyPostedFilesAllSigsIshFilePath);
        emit doneWatchingSigsFileAndPostingChangesToUsenet(false);
        return false;
    }

    QTextStream saveFileStream(&saveFile);
    QSetIterator<RecursiveCustomDetachedSignaturesFileMeta> postedFilesIterator(m_FilesAlreadyPostedOnUsenet);
    while(postedFilesIterator.hasNext())
    {
        const RecursiveCustomDetachedSignaturesFileMeta &currentPathAndSig = postedFilesIterator.next();
        saveFileStream << currentPathAndSig; //TODOoptimization: we really only need the path, not the sigs... fuckit since I want to use qset.subtract/etc because it looks sexy and i never have before (new = fun)
    }
    saveFileStream.flush();
    if(!saveFile.commit())
    {
        emit e("failed to commit list of already posted files to disk for next time: " + m_AlreadyPostedFilesAllSigsIshFilePath);
        emit doneWatchingSigsFileAndPostingChangesToUsenet(false);
        return false;
    }
    emit o("successfully committed list of already posted files to disk for next time");
    return true;
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
void WatchSigsFileAndPostChangesToUsenet::startWatchingSigsFileAndPostChangesToUsenet(const QString &sigsFilePathToWatch, const QString &dirCorrespondingToSigsFile/*, const QString &dataDirForKeepingTrackOfAlreadyPostedFiles*/, const QString &authUser, const QString &authPass, const QString &portString, const QString &server)
{
    QDir dirCorrespondingToSigsFileInstance(dirCorrespondingToSigsFile); //I know QDir has implicit conversion from QString, but I don't think the old connect syntax can do implicit conversion. The new connect syntax can, but then I lose overloads I think :-/, maybe wrong here fuckit
    //QDir dataDirForKeepingTrackOfAlreadyPostedFilesInstance(dataDirForKeepingTrackOfAlreadyPostedFiles);
    startWatchingSigsFileAndPostChangesToUsenet(sigsFilePathToWatch, dirCorrespondingToSigsFileInstance/*, dataDirForKeepingTrackOfAlreadyPostedFilesInstance*/, authUser, authPass, portString, server);
}
void WatchSigsFileAndPostChangesToUsenet::startWatchingSigsFileAndPostChangesToUsenet(const QString &sigsFilePathToWatch /*this was a QIODevice, but QFsw can't watch an iodevice (readyRead() misleads an amateur :-P)*/, const QDir &dirCorrespondingToSigsFile/*, const QDir &dataDirForKeepingTrackOfAlreadyPostedFiles*/, const QString &authUser, const QString &authPass, const QString &portString, const QString &server)
{
    m_UsenetAuthUsername = authUser;
    m_UsenetAuthPassword = authPass;
    m_PortString = portString;
    m_UsenetServer = server;
    m_CleanQuitRequested = false;

    m_SigsFileWatcher->removePaths(m_SigsFileWatcher->files());
    if(!startWatchingSigsFile(sigsFilePathToWatch))
        return;
    if(!dirCorrespondingToSigsFile.exists())
        EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("dir corresponding to sigsfile does not exist:" + dirCorrespondingToSigsFile.absolutePath())

    //if(!dataDirForKeepingTrackOfAlreadyPostedFiles.exists())
    //    EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("data dir does not exist:" + dataDirForKeepingTrackOfAlreadyPostedFiles.absolutePath())
    //m_AlreadyPostedFilesAllSigsIshFileName = dirCorrespondingToSigsFile.absolutePath() + QDir::separator() + "WatchSigsFileAndPostChangesToUsenet_AlreadyPostedFiles_Db.ini"; //TODOoptional: how to use the same dir as qsettings :(? woot QStandardPaths::AppDataLocation -- mfw QStandardPaths::findExecutable TODOoptional use it to 'find' postnews and bins in other apps (but then again, might as well let QProcess do it fuckit why do more work for nothing) :-D. TODOreq: make specifying the dir (OR PERHAPS I SHOULD DO FILE ITSELF? not dir (dir implied ofc)) optional
    const QDir &pathToAppLocalDataDirWhichMightNotExist = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if(!pathToAppLocalDataDirWhichMightNotExist.exists())
    {
        if(!pathToAppLocalDataDirWhichMightNotExist.mkpath(pathToAppLocalDataDirWhichMightNotExist.absolutePath()))
            EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("failed to make app local data dir: " + pathToAppLocalDataDirWhichMightNotExist.absolutePath())
    }
    m_AlreadyPostedFilesAllSigsIshFilePath = pathToAppLocalDataDirWhichMightNotExist.absolutePath() + QDir::separator() + "WatchSigsFileAndPostChangesToUsenet_AlreadyPostedFiles_UNSORTED-BTW_Db.ini";
    //it's cleared in the assign statement in readInAlreadyPostedFilesSoWeDontDoublePost: m_FilesAlreadyPostedOnUsenet.clear();
    if(!readInAlreadyPostedFilesSoWeDontDoublePost())
        return;

    readInSigsFileAndPostAllNewEntries(sigsFilePathToWatch); //start posting right away
}
void WatchSigsFileAndPostChangesToUsenet::quitCleanly()
{
    m_CleanQuitRequested = true;
    //TODOmb: remove watched file from file watcher?
    postAnEnqueuedFileIfNotAlreadyPostingOne_OrQuitIfCleanQuitRequested(); //handles the case where there's a post in progress
}
void WatchSigsFileAndPostChangesToUsenet::handleSigsFileChanged(const QString &sigsFilePath)
{
    if(!startWatchingSigsFile(sigsFilePath))
        return;
    readInSigsFileAndPostAllNewEntries(sigsFilePath);
}
void WatchSigsFileAndPostChangesToUsenet::handlePostnewsProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    //TODOoptional: detect/handle "441 Posting Failed. Message-ID is not unique E1" with exit code of 2
    if(exitCode != 0 || exitStatus != QProcess::NormalExit)
        EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("postnews exitted abnormally with exit code: " + QString::number(exitCode))
    RecursiveCustomDetachedSignaturesFileMeta postedFile(*m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet.data());
    emit o("file posted to usenet: " + postedFile.FilePath);
    m_FilesEnqueuedForPostingToUsenet.remove(postedFile);
    m_FilesAlreadyPostedOnUsenet.insert(postedFile);
    m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet.reset();
    postAnEnqueuedFileIfNotAlreadyPostingOne_OrQuitIfCleanQuitRequested();
}
