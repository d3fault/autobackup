#include "watchsigsfileandpostchangestousenet.h"

#include <QMutableHashIterator>
#include <QCryptographicHash>
#include <QMimeDatabase>
#include <QProcess>
#include <QStandardPaths>
#include <QSaveFile>
#include <QSetIterator>
#include <QTemporaryFile>
#include <QDateTime>

#define WatchSigsFileAndPostChangesToUsenet_MAX_SINGLE_PART_SIZE_ANDOR_SPLIT_FILE_VOLUME_SIZE 250000 //350 is what I really want, but gotta take into account the b64 enc xD
#define WatchSigsFileAndPostChangesToUsenet_SEVENZIP_BIN "7z"
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
    postToUsenet(nextFile);
}
//TODOoptimization: compress next file while current file is posting
//TODooptional: retry with exponential backoff (OT: sftp uploader should too)
//TODOoptional: parity. spent way too much time researching/considering yEnc+parity etc. fuck it
void WatchSigsFileAndPostChangesToUsenet::postToUsenet(const RecursiveCustomDetachedSignaturesFileMeta &nextFile) //TODOreq: relative path isn't being posted anywhere, maybe I should just not care and keep that stuff to the .nzb only?
{
    m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet.reset(new RecursiveCustomDetachedSignaturesFileMeta(nextFile));
    QFileInfo fileInfo(nextFile.FilePath);
    if(!fileInfo.isReadable())
        EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("file not readable: " + nextFile.FilePath);
    const QString &fileNameOnly = fileInfo.fileName();
    if(fileInfo.size() > WatchSigsFileAndPostChangesToUsenet_MAX_SINGLE_PART_SIZE_ANDOR_SPLIT_FILE_VOLUME_SIZE)
    {
        //split into 7z volumes, post each as mime shit (no sig attachment, since sig in archive)
        m_FileCurrentlyBeingPostedToUsenetVolumesTempDir_OrNullIfFileCurrentlyBeingPostedDidntNeedToBeSplit.reset(new QTemporaryDir()); //TO DOneoptimization. reset(0) sooner! as soon as each file in the temp dir is posted, but at least this is safe here if i forget that :)
        if(!m_FileCurrentlyBeingPostedToUsenetVolumesTempDir_OrNullIfFileCurrentlyBeingPostedDidntNeedToBeSplit->isValid())
            EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("failed to get temp dir for 7-zip volume'ing: " + nextFile.FilePath);
        QString sevenZipOutDir_WithSlashAppended = appendSlashIfNeeded(m_FileCurrentlyBeingPostedToUsenetVolumesTempDir_OrNullIfFileCurrentlyBeingPostedDidntNeedToBeSplit->path());
        QProcess sevenZipProcess;
        sevenZipProcess.setProcessChannelMode(QProcess::ForwardedChannels);
        QStringList sevenZipArgs;
        QString sevenZipVolumeBaseFilePath = sevenZipOutDir_WithSlashAppended + fileNameOnly + ".7z";
        QString sevenZipVolumeSizeArg = "-v" + QString::number(WatchSigsFileAndPostChangesToUsenet_MAX_SINGLE_PART_SIZE_ANDOR_SPLIT_FILE_VOLUME_SIZE);
        QString sigFilePath = sevenZipOutDir_WithSlashAppended + fileNameOnly + ".asc";
        QFile sigFile(sigFilePath); //7-zip can read from stdin, but only 1 file blah, so I have to write this to disk xD. TODOreq: don't 'post' this when iterating the sevenZipOutputDir (since it's already IN the archive)
        if(!sigFile.open(QIODevice::WriteOnly | QIODevice::Text))
            EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("failed to write sig to disk for 7-zipping alongside: " + nextFile.FilePath)
        QTextStream sigFileStream(&sigFile);
        sigFileStream << nextFile.GpgSignature;
        sigFileStream.flush();
        if(!sigFile.flush())
            EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("failed to flush sig file written to disk for 7-zipping alongside: " + nextFile.FilePath)
        sigFile.close(); //it'll get cleaned up when the temp dir is
        sevenZipArgs << "a" << sevenZipVolumeBaseFilePath << nextFile.FilePath << sigFilePath << sevenZipVolumeSizeArg << "-t7z" << "-m0=lzma" << "-mx=1";
        sevenZipProcess.start(WatchSigsFileAndPostChangesToUsenet_SEVENZIP_BIN, sevenZipArgs, QIODevice::ReadOnly);
        if(!sevenZipProcess.waitForStarted(-1))
            EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("7-zip failed to start with args: " + sevenZipArgs.join(", "))
        if(!sevenZipProcess.waitForFinished(-1))
            EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("7-zip failed to finish with args: " + sevenZipArgs.join(", "))
        if(sevenZipProcess.exitCode() != 0 || sevenZipProcess.exitStatus() != QProcess::NormalExit)
            EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("7-zip exitted abnormally with exit code: " + QString::number(sevenZipProcess.exitCode()) + " -- args: " + sevenZipArgs.join(", "))
        //volumes created
        postNextVolumePartInDir_OrContinueOntoNextFullFileIfAllPartsOfCurrentFileHaveBeenPosted();
        return;
    }
    else if(fileInfo.size() > 0)
    {
        //post as single file with sig attachment
        beginPostingToUsenetAfterBase64encoding(fileInfo, nextFile.GpgSignature);
        return;
    }
    else //file size < 1
    {
        //skip. no point in posting
        handleFullFilePostedToUsenet(); //lies that it was posted xD TODOoptional: dont
        return;
    }
}
void WatchSigsFileAndPostChangesToUsenet::postNextVolumePartInDir_OrContinueOntoNextFullFileIfAllPartsOfCurrentFileHaveBeenPosted()
{
    if(m_FileCurrentlyBeingPostedToUsenetVolumesTempDirIterator.isNull())
        m_FileCurrentlyBeingPostedToUsenetVolumesTempDirIterator.reset(new QDirIterator(m_FileCurrentlyBeingPostedToUsenetVolumesTempDir_OrNullIfFileCurrentlyBeingPostedDidntNeedToBeSplit->path(), (QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden)));
    while(m_FileCurrentlyBeingPostedToUsenetVolumesTempDirIterator->hasNext())
    {
        m_FileCurrentlyBeingPostedToUsenetVolumesTempDirIterator->next();
        const QFileInfo &currentFileInfo = m_FileCurrentlyBeingPostedToUsenetVolumesTempDirIterator->fileInfo();
        if(!currentFileInfo.isFile() || currentFileInfo.suffix().toLower() == "asc" /*TODOoptional: hack that this asc check is here*/)
            continue;
        beginPostingToUsenetAfterBase64encoding(currentFileInfo, QString(), QString("application/x-7z-compressed"));
        return;
    }
    //done with all this file's parts
    m_FileCurrentlyBeingPostedToUsenetVolumesTempDirIterator.reset();
    m_FileCurrentlyBeingPostedToUsenetVolumesTempDir_OrNullIfFileCurrentlyBeingPostedDidntNeedToBeSplit.reset(); //deletes underlying temp dir/files
    //and then continue onto the next ACTUAL file
    handleFullFilePostedToUsenet();
}
void WatchSigsFileAndPostChangesToUsenet::beginPostingToUsenetAfterBase64encoding(const QFileInfo &fileInfo, const QString &gpgSignature_OrEmptyStringIfNotToAttachOne, const QString &mimeType_OrEmptyStringIfToFigureItOut) //TODOoptional: when bored on rainy day, post all the MyBrain-PublicFiles.tc files individually
{
    const QString &absoluteFilePath = fileInfo.absoluteFilePath();
    QFile file(absoluteFilePath);
    if(!file.open(QIODevice::ReadOnly))
        EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("failed to open file: " + fileInfo.absoluteFilePath())

    QByteArray body;
    QByteArray fileContentsMd5;

    {
        const QByteArray &fileContents = file.readAll(); //1x full copy in memory
        const QByteArray &fileContentsBase64 = fileContents.toBase64(); //2x full copies in memory
        body = wrap(fileContentsBase64, 72); //3x full copies in memory
        fileContentsMd5 = QCryptographicHash::hash(fileContents, QCryptographicHash::Sha1);
    }//2 of the full copies in memory go out of scope <3

    QByteArray contentMd5Base64 = fileContentsMd5.toBase64();
    QByteArray boundary;
    QByteArray gpgSigBA = gpgSignature_OrEmptyStringIfNotToAttachOne.toLatin1();

    do
    {
        boundary = generateRandomAlphanumericBytes(qrand() % 10);

    }while(body.contains(boundary) || ((!gpgSignature_OrEmptyStringIfNotToAttachOne.isEmpty()) && gpgSigBA.contains(boundary)));

    QByteArray fileNameOnly = fileInfo.fileName().toLatin1();
    QByteArray mime;
    if(!mimeType_OrEmptyStringIfToFigureItOut.isEmpty())
        mime = mimeType_OrEmptyStringIfToFigureItOut.toLatin1();
    else
        mime = m_MimeDatabase.mimeTypeForFile(fileInfo).name().toLatin1();

    //TODOreq: file splitting ('hat' will make large videos every day, and there's also the relics to consider)
    //TODOreq: message-id (dep file splitting)

    QByteArray messageID = generateRandomAlphanumericBytes(qrand() % 10) + "@" + generateRandomAlphanumericBytes(qrand() % 15);
    QByteArray post(
                    //"From: d3fault@d3fault.net\r\n"
                    "From: " + generateRandomAlphanumericBytes(qrand() % 12) + " <" + generateRandomAlphanumericBytes(qrand() % 12) + "@" + generateRandomAlphanumericBytes(qrand() % 15) + ".com>\r\n"
                    "Newsgroups: alt.binaries.boneless\r\n"
                    //"Subject: d3fault.net/binary/" + nextFile.FilePath.toLatin1() + "\r\n"
                    "Subject: " + generateRandomAlphanumericBytes(qrand() % 32) + "\r\n"
                    "Message-ID: <" + messageID + ">\r\n"
                    //"Organization: d3fault\r\n"
                    "Organization: " + generateRandomAlphanumericBytes(qrand() % 20) + "\r\n"
                    "Mime-Version 1.0\r\n"
                    "Content-Type: multipart/signed; boundary=" + boundary + "; protocol=\"application/pgp-signature\"\r\n"
                    "\r\n"
                    "--" + boundary + "\r\n"
                    "Content-Type: " + mime + "; name=\"" + fileNameOnly + "\"\r\n"
                    "Content-Transfer-Encoding: base64\r\n"
                    "Content-Disposition: inline; filename=\"" + fileNameOnly + "\"\r\n"
                    "Content-MD5: " + contentMd5Base64 + "\r\n"
                    "\r\n"
                    + body + "\r\n"
                    "\r\n"
                    "--" + boundary + "\r\n");
    if(!gpgSignature_OrEmptyStringIfNotToAttachOne.isEmpty())
    {
        post.append(
                    "Content-Type: application/pgp-signature\r\n" //TODOoptional: micalg=???
                    "Content-Disposition: attachment; filename=\"" + fileNameOnly + ".asc\"\r\n"
                    "\r\n"
                    + gpgSigBA + "\r\n"
                    "\r\n"
                    "--" + boundary + "\r\n"
                   ); //TODOoptional: customizeable parts of this  from command line ofc lol. TODOmb: Message-ID (even if it's not an sha1 or whatever, it'd still be good to know it so we don't have to query it. We could start with sha1 and then keep trying different values if it doesn't work)
    }
    emit o("Trying to post: " + absoluteFilePath);

    QStringList postnewsArgs;
    postnewsArgs << "--verbose" << ("--user=" + m_UsenetAuthUsername) << ("--pass=" + m_UsenetAuthPassword) << ("--port=" + m_PortString) << m_UsenetServer;
    m_PostnewsProcess->start(WatchSigsFileAndPostChangesToUsenet_POSTNEWS_BIN, postnewsArgs);
    if(!m_PostnewsProcess->write(post))
        EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("failed to write post to postnews' stdin, the file at: " + absoluteFilePath);
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
QByteArray WatchSigsFileAndPostChangesToUsenet::generateRandomAlphanumericBytes(int numBytesToGenerate)
{
    static qint64 nonce = 0; //TO DOnemb: global nonce? -- not thread safe (err, re-entrant), but doesn't matter <3
    //QByteArray randomSeed("-whatever_the-laws_of-physics_allow-" + currentDateTime + QString::number(nonce++).toLatin1()); //Underscores are necesary because gpg sigs would accidentally have a boundary in them. //fileContentsMd5.toHex(); //heh -- TODOreq: consider randomizing this and from field and subject (especially subject, since apparently subjects are filtered guh i can't guarantee my filenames won't contain filtered words). newsgroups are structured like pyramids, therefore are censored/etc (maybe time will tell me that this posting was all a waste of time)
    //QByteArray random = QCryptographicHash::hash(randomSeed, QCryptographicHash::Sha512);
    static const char alphaNumericRange[] = "0123456789"
                                            "abcdefghijklmnopqrstuvwxyz"
                                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    qsrand(QDateTime::currentMSecsSinceEpoch()-(nonce++)); //woot "using" network latency + cpu compress latency + disk latency to seed the prn lel
    QByteArray ret;
    while(ret.size() < numBytesToGenerate)
    {
        ret.append(alphaNumericRange[qrand() % (sizeof(alphaNumericRange)-1)]);
    }
    return ret;
}
void WatchSigsFileAndPostChangesToUsenet::handleFullFilePostedToUsenet() //full as in "all of a file's parts" OR "a single file that was not split into parts". TODOreq: single file calls this when done
{
    RecursiveCustomDetachedSignaturesFileMeta postedFile(*m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet.data());
    emit o("file posted to usenet: " + postedFile.FilePath);
    m_FilesEnqueuedForPostingToUsenet.remove(postedFile);
    m_FilesAlreadyPostedOnUsenet.insert(postedFile);
    m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet.reset();
    postAnEnqueuedFileIfNotAlreadyPostingOne_OrQuitIfCleanQuitRequested();
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
    //TODOreq: detect/handle "441 Posting Failed. Message-ID is not unique E1" with exit code of 2
    if(exitCode != 0 || exitStatus != QProcess::NormalExit)
        EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("postnews exitted abnormally with exit code: " + QString::number(exitCode))

    if(!m_FileCurrentlyBeingPostedToUsenetVolumesTempDir_OrNullIfFileCurrentlyBeingPostedDidntNeedToBeSplit.isNull())
    {
        postNextVolumePartInDir_OrContinueOntoNextFullFileIfAllPartsOfCurrentFileHaveBeenPosted();
        return;
    }
    handleFullFilePostedToUsenet();
}
