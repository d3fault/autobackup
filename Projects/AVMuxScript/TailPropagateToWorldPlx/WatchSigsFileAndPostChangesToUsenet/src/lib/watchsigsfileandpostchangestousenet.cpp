#include "watchsigsfileandpostchangestousenet.h"

#include <QSettings>
#include <QMutableHashIterator>
#include <QCryptographicHash>
#include <QMimeDatabase>
#include <QProcess>
#include <QStandardPaths>
#include <QSetIterator>
#include <QTemporaryFile>
#include <QDateTime>
#include <QTimer>
#include <QtMath>

#define WatchSigsFileAndPostChangesToUsenet_MAX_SINGLE_PART_SIZE_ANDOR_SPLIT_FILE_VOLUME_SIZE 250000 //350kb is what I really want, but gotta take into account the b64 enc xD
#define WatchSigsFileAndPostChangesToUsenet_SEVENZIP_BIN "7z"
#define WatchSigsFileAndPostChangesToUsenet_POSTNEWS_BIN "postnews"
#define WatchSigsFileAndPostChangesToUsenet_MIN_DELAY_BEFORE_RETRYING_MS 5000
#define EEEEEEEE_WatchSigsFileAndPostChangesToUsenet(msg) { emit e(msg); emit doneWatchingSigsFileAndPostingChangesToUsenet(false); return; }

WatchSigsFileAndPostChangesToUsenet::WatchSigsFileAndPostChangesToUsenet(QObject *parent)
    : QObject(parent)
    , m_SigsFileWatcher(new QFileSystemWatcher(this)) //the best reason for pointers: qobject parenting (which makes moveToThread work). If it's not a pointer (a stack member) and sets 'this' as parent, then it gets double deleted xD
    , m_RecursiveCustomDetachedSignatures(new RecursiveCustomDetachedSignatures(this))
    , m_PostnewsProcess(new QProcess(this))
    , m_RetryWithExponentialBackoffTimer(new QTimer(this))
{
    m_PostnewsProcess->setProcessChannelMode(QProcess::ForwardedOutputChannel);

    connect(m_SigsFileWatcher, SIGNAL(fileChanged(QString)), this, SLOT(handleSigsFileChanged(QString)));
    connect(m_RecursiveCustomDetachedSignatures, SIGNAL(e(QString)), this, SIGNAL(e(QString)));
    connect(m_PostnewsProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(handlePostnewsProcessFinished(int,QProcess::ExitStatus)));

    m_RetryWithExponentialBackoffTimer->setSingleShot(true);
    connect(m_RetryWithExponentialBackoffTimer, SIGNAL(timeout()), this, SLOT(generateMessageIdAndPostToUsenet())); //we don't need a new message id, but eh simplicity is simple
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
bool WatchSigsFileAndPostChangesToUsenet::ensureExistsAndIsExecutable(const QString &binaryToVerify)
{
    const QString &binaryFilePath = QStandardPaths::findExecutable(binaryToVerify);
    if(!binaryFilePath.isEmpty())
    {
        QFileInfo binaryFileInfo(binaryFilePath); //mb not necessary, idk what findExecutable does aside from existence verification
        if(binaryFileInfo.isExecutable())
        {
            return true;
        }
    }
    emit e("could not find executable, or it didn't have executable bit set: " + binaryToVerify);
    emit doneWatchingSigsFileAndPostingChangesToUsenet(false);
    return false;
}
void WatchSigsFileAndPostChangesToUsenet::readInSigsFileAndPostAllNewEntries(const QString &sigsFilePath)
{
    QFile sigsFileToWatch(sigsFilePath);
    if(!sigsFileToWatch.open(QIODevice::ReadOnly | QIODevice::Text))
        EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("failed to open sigs file for reading: " + sigsFilePath)
    QHash<QString /*file path*/, RecursiveCustomDetachedSignaturesFileMeta /*file meta*/> sigsFromSigFile;
    if(!m_RecursiveCustomDetachedSignatures->readInAllSigsFromSigFile(&sigsFileToWatch, &sigsFromSigFile))
        EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("failed to read in all sigs from file:" + sigsFilePath);

    //now remove the files already enqueued or posted
    QMutableHashIterator<QString, RecursiveCustomDetachedSignaturesFileMeta> newSigsIterator(sigsFromSigFile);
    while(newSigsIterator.hasNext())
    {
        newSigsIterator.next();
        if(m_FilesEnqueuedForPostingToUsenet.contains(newSigsIterator.key()) || m_AlreadyPostedFiles->contains(newSigsIterator.key()))
            newSigsIterator.remove();
    }
    if(!checkAlreadyPostedFilesForError())
        return;

    //TODOoptimization: OLD BUT STILL SEMI-RELEVANT (old as fuck posted files should be moved out of sight): I either have to keep the entire fs hierarchy+sigs in memory (only a few mb at the moment, but it will grow!!!), or I have to read them from disk every time the sigsfile being watched changes... idfk. There are clever ways to solve that problem ofc (and it's worth noting we don't NEED the sigs of the already posted files in memory, only need the file paths (this alone is probably ~80% of that memory now freed xD))...

    //at this point, sigsFromSigFile only contains new entries. so enqueue them
    m_FilesEnqueuedForPostingToUsenet.unite(sigsFromSigFile); //arbitrary posting order (QHash), fuck it. too lazy to sort by timestamp
    postAnEnqueuedFileIfNotAlreadyPostingOne_OrQuitIfCleanQuitRequested();
}
void WatchSigsFileAndPostChangesToUsenet::postAnEnqueuedFileIfNotAlreadyPostingOne_OrQuitIfCleanQuitRequested()
{
    if(!m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet.isNull())
        return;
    if(m_CleanQuitRequested)
    {
        if(!checkAlreadyPostedFilesForError())
            return;
        emit doneWatchingSigsFileAndPostingChangesToUsenet(true);
        return;
    }
    if(m_FilesEnqueuedForPostingToUsenet.isEmpty())
    {
        checkAlreadyPostedFilesForError();
        return;
    }
    const RecursiveCustomDetachedSignaturesFileMeta &nextFile = *(m_FilesEnqueuedForPostingToUsenet.begin());
    postToUsenet(nextFile);
}
//TODOoptimization: compress next file while current file is posting
//TODOoptional: parity. spent way too much time researching/considering yEnc+parity etc. fuck it, 7z it is
void WatchSigsFileAndPostChangesToUsenet::postToUsenet(const RecursiveCustomDetachedSignaturesFileMeta &nextFile) //TODOreq: relative path isn't being posted anywhere, maybe I should just not care and keep that stuff to the .nzb only?
{
    m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet.reset(new RecursiveCustomDetachedSignaturesFileMetaAndListOfMessageIDs(nextFile));
    QFileInfo fileInfo(m_DirCorrespondingToSigsFile.absolutePath() + QDir::separator() + nextFile.FilePath);
    const QString &absoluteFilePath = fileInfo.absoluteFilePath();
    if(!fileInfo.isReadable())
        EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("file not readable: " + absoluteFilePath);
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
        QFile sigFile(sigFilePath); //7-zip can read from stdin, but only 1 file blah, so I have to write this to disk xD. TO DOnereq: don't 'post' this sig file when iterating the sevenZipOutputDir (since it's already IN the archive)
        if(!sigFile.open(QIODevice::WriteOnly | QIODevice::Text))
            EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("failed to write sig to disk for 7-zipping alongside: " + nextFile.FilePath)
        QTextStream sigFileStream(&sigFile);
        sigFileStream << nextFile.GpgSignature;
        sigFileStream.flush();
        if(!sigFile.flush())
            EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("failed to flush sig file written to disk for 7-zipping alongside: " + nextFile.FilePath)
        sigFile.close(); //it'll get cleaned up when the temp dir is
        sevenZipArgs << "a" << sevenZipVolumeBaseFilePath << absoluteFilePath << sigFilePath << sevenZipVolumeSizeArg << "-t7z" << "-m0=lzma" << "-mx=1";
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
        fileContentsMd5 = QCryptographicHash::hash(fileContents, QCryptographicHash::Md5);
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

    //TODOreq: message-id. retrying

    UsenetPostDetails postDetails;
    postDetails.Boundary = boundary;
    postDetails.Mime = mime;
    postDetails.FileNameOnly = fileNameOnly;
    postDetails.ContentMd5Base64 = contentMd5Base64;
    postDetails.Body = body;
    postDetails.AttachGpgSig = (!gpgSignature_OrEmptyStringIfNotToAttachOne.isEmpty());
    postDetails.GpgSigMaybe = gpgSigBA;
    postDetails.AbsoluteFilePath = absoluteFilePath;

    m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet->PostInProgressDetails = postDetails;
    generateMessageIdAndPostToUsenet();
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
    //QByteArray randomSeed("-whatever_the-laws_of-physics_allow-" + currentDateTime + QString::number(nonce++).toLatin1()); //Underscores are necesary because gpg sigs would accidentally have a boundary in them. //fileContentsMd5.toHex(); //heh -- TO DOnereq: consider randomizing this and from field and subject (especially subject, since apparently subjects are filtered guh i can't guarantee my filenames won't contain filtered words). newsgroups are structured like pyramids, therefore are censored/etc (maybe time will tell me that this posting was all a waste of time)
    //QByteArray random = QCryptographicHash::hash(randomSeed, QCryptographicHash::Sha512);
    static const char alphaNumericRange[] = "0123456789"
                                            "abcdefghijklmnopqrstuvwxyz"
                                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    qsrand(QDateTime::currentMSecsSinceEpoch()-(nonce++)); //woot "using" network latency + cpu compress latency + disk latency to seed the prn lel
    QByteArray ret;
    int numBytesToGenerateActual = qMax(4, numBytesToGenerate); //generate 4 bytes minimum
    while(ret.size() < numBytesToGenerateActual)
    {
        ret.append(alphaNumericRange[qrand() % (sizeof(alphaNumericRange)-1)]);
    }
    return ret;
}
void WatchSigsFileAndPostChangesToUsenet::generateMessageIdAndPostToUsenet()
{
    m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet->PostInProgressDetails.MessageId = generateRandomAlphanumericBytes(qrand() % 10) + "@" + generateRandomAlphanumericBytes(qrand() % 15);
    const UsenetPostDetails &postDetails = m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet->PostInProgressDetails;
    QByteArray post(
                    //"From: d3fault@d3fault.net\r\n"
                    "From: " + generateRandomAlphanumericBytes(qrand() % 12) + " <" + generateRandomAlphanumericBytes(qrand() % 12) + "@" + generateRandomAlphanumericBytes(qrand() % 15) + ".com>\r\n"
                    "Newsgroups: alt.binaries.boneless\r\n"
                    //"Subject: d3fault.net/binary/" + nextFile.FilePath.toLatin1() + "\r\n"
                    "Subject: " + generateRandomAlphanumericBytes(qrand() % 32) + "\r\n"
                    "Message-ID: <" + postDetails.MessageId + ">\r\n"
                    //"Organization: d3fault\r\n"
                    "Organization: " + generateRandomAlphanumericBytes(qrand() % 20) + "\r\n"
                    "Mime-Version 1.0\r\n"
                    "Content-Type: multipart/signed; boundary=" + postDetails.Boundary + "; protocol=\"application/pgp-signature\"\r\n"
                    "\r\n"
                    "--" + postDetails.Boundary + "\r\n"
                    "Content-Type: " + postDetails.Mime + "; name=\"" + postDetails.FileNameOnly + "\"\r\n"
                    "Content-Transfer-Encoding: base64\r\n"
                    "Content-Disposition: inline; filename=\"" + postDetails.FileNameOnly + "\"\r\n"
                    "Content-MD5: " + postDetails.ContentMd5Base64 + "\r\n"
                    "\r\n"
                    + postDetails.Body + "\r\n"
                    "\r\n"
                    "--" + postDetails.Boundary + "\r\n");
    if(postDetails.AttachGpgSig)
    {
        post.append(
                    "Content-Type: application/pgp-signature\r\n" //TODOoptional: micalg=???
                    "Content-Disposition: attachment; filename=\"" + postDetails.FileNameOnly + ".asc\"\r\n"
                    "\r\n"
                    + postDetails.GpgSigMaybe + "\r\n"
                    "\r\n"
                    "--" + postDetails.Boundary + "\r\n"
                   ); //TODOoptional: customizeable parts of this  from command line ofc lol. TODOmb: Message-ID (even if it's not an sha1 or whatever, it'd still be good to know it so we don't have to query it. We could start with sha1 and then keep trying different values if it doesn't work)
    }
    emit o("Trying to post: " + postDetails.AbsoluteFilePath);

    QStringList postnewsArgs;
    postnewsArgs << "--verbose" << ("--user=" + m_UsenetAuthUsername) << ("--pass=" + m_UsenetAuthPassword) << ("--port=" + m_PortString) << m_UsenetServer;
    m_PostnewsProcess->start(WatchSigsFileAndPostChangesToUsenet_POSTNEWS_BIN, postnewsArgs);
    if(!m_PostnewsProcess->write(post))
        EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("failed to write post to postnews' stdin, the file at: " + postDetails.AbsoluteFilePath);
    m_PostnewsProcess->closeWriteChannel();
}
void WatchSigsFileAndPostChangesToUsenet::handleFullFilePostedToUsenet() //full as in "all of a file's parts" OR "a single file that was not split into parts"
{
    const RecursiveCustomDetachedSignaturesFileMeta &postedFile = m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet->FileMeta;
    emit o("file posted to usenet: " + postedFile.FilePath);
    m_FilesEnqueuedForPostingToUsenet.remove(postedFile.FilePath);
    //m_FilesAlreadyPostedOnUsenet_AndTheirMessageIDs.insert(postedFile.FilePath, m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet->MessageIDs);
    //QSettings alreadyPostedFilesSerializer(m_AlreadyPostedFiles, QSettings::IniFormat); //TODOblah: the only thing that should happen when the queue "goes empty" is that we call sync and check the status of the settings :)
    m_AlreadyPostedFiles->setValue(postedFile.FilePath, m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet->SuccessfullyPostedMessageIDs);
    m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet.reset();
    postAnEnqueuedFileIfNotAlreadyPostingOne_OrQuitIfCleanQuitRequested();
}
bool WatchSigsFileAndPostChangesToUsenet::checkAlreadyPostedFilesForError()
{
    m_AlreadyPostedFiles->sync();
    if(m_AlreadyPostedFiles->status() != QSettings::NoError)
    {
        //TODOmb: maybe set a flag so that no more posts are attempted. this is eh racy, we could get a write error at the same time as a sigs file change detected. we're already fucked at that point tho so maybe who cares?
        emit e("Error syncing/flushing (QSettings) the already posted files (either for initial reading, or for saving for next time) to: " + m_AlreadyPostedFiles->fileName() + " error status: " + QString::number(m_AlreadyPostedFiles->status()));
        emit doneWatchingSigsFileAndPostingChangesToUsenet(false);
        return false;
    }
    return true;
}
//TODOoptional: when recursive gpg signer makes the file, it's sorted by the filepath. I have no need to sort it like that (but could easily), so I won't (THINK OF THE CLOCK CYCLES!!)
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
    m_NumFailedPostAttemptsInArow = 0; //quint64 so we can retry with exponential backoff until the end of time! aww nvm QTimer overflows long before that :(

    m_SigsFileWatcher->removePaths(m_SigsFileWatcher->files());
    if(!startWatchingSigsFile(sigsFilePathToWatch))
        return;
    if(!dirCorrespondingToSigsFile.exists())
        EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("dir corresponding to sigsfile does not exist:" + dirCorrespondingToSigsFile.absolutePath())
    m_DirCorrespondingToSigsFile = dirCorrespondingToSigsFile;

    //if(!dataDirForKeepingTrackOfAlreadyPostedFiles.exists())
    //    EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("data dir does not exist:" + dataDirForKeepingTrackOfAlreadyPostedFiles.absolutePath())
    //m_AlreadyPostedFilesAllSigsIshFileName = dirCorrespondingToSigsFile.absolutePath() + QDir::separator() + "WatchSigsFileAndPostChangesToUsenet_AlreadyPostedFiles_Db.ini"; //TODOoptional: how to use the same dir as qsettings :(? woot QStandardPaths::AppDataLocation -- mfw QStandardPaths::findExecutable TODOoptional use it to 'find' postnews and bins in other apps (but then again, might as well let QProcess do it fuckit why do more work for nothing) :-D. TODOmb: make specifying the file optional
    const QDir &pathToAppLocalDataDirWhichMightNotExist = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if(!pathToAppLocalDataDirWhichMightNotExist.exists())
    {
        if(!pathToAppLocalDataDirWhichMightNotExist.mkpath(pathToAppLocalDataDirWhichMightNotExist.absolutePath()))
            EEEEEEEE_WatchSigsFileAndPostChangesToUsenet("failed to make app local data dir: " + pathToAppLocalDataDirWhichMightNotExist.absolutePath())
    }
    m_AlreadyPostedFiles.reset(new QSettings(pathToAppLocalDataDirWhichMightNotExist.absolutePath() + QDir::separator() + "AlreadyPostedFiles_AndTheirMessageIDs_Db.ini", QSettings::IniFormat));
    //it's cleared in the assign statement in readInAlreadyPostedFilesSoWeDontDoublePost: m_FilesAlreadyPostedOnUsenet.clear();
    //if(!readInAlreadyPostedFilesSoWeDontDoublePost())
    //    return;

    //check postnews and 7z are executable
    if(!ensureExistsAndIsExecutable(WatchSigsFileAndPostChangesToUsenet_POSTNEWS_BIN))
        return;
    if(!ensureExistsAndIsExecutable(WatchSigsFileAndPostChangesToUsenet_SEVENZIP_BIN))
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
    const QString &postNewsStdErr = m_PostnewsProcess->readAllStandardError();
    emit e(postNewsStdErr); //"forwarded channels" -- but we want to parse it also :)
    if(exitCode == 2 && postNewsStdErr.contains("441 Posting Failed. Message-ID is not unique E1"))
    {
        generateMessageIdAndPostToUsenet(); //re-post, that is
        return;
    }
    //TODOoptional: arg for max retries in a row before giving up completely. maybe i should use this by default and just set it to something pretty damn high (taking into account the exponential'ness (so like 30 rofl <3 exponents))
    if(exitCode != 0 || exitStatus != QProcess::NormalExit)
    {
        //regardless of the error type, retry with exponential backoff. I'll see it failing and intervene
        ++m_NumFailedPostAttemptsInArow;
        int mult = qPow(2, m_NumFailedPostAttemptsInArow);
        int sleepForMs = WatchSigsFileAndPostChangesToUsenet_MIN_DELAY_BEFORE_RETRYING_MS * mult;
        m_RetryWithExponentialBackoffTimer->start(sleepForMs);
        emit e("post failed, retrying in " + QString::number(sleepForMs/1000) + " seconds...");
        return;
    }
    m_NumFailedPostAttemptsInArow = 0;

    m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet->SuccessfullyPostedMessageIDs.append(QString::fromLatin1(m_FileCurrentlyBeingPostedToUsenet_OrNullIfNotCurrentlyPostingAFileToUsenet->PostInProgressDetails.MessageId));

    if(!m_FileCurrentlyBeingPostedToUsenetVolumesTempDir_OrNullIfFileCurrentlyBeingPostedDidntNeedToBeSplit.isNull())
    {
        postNextVolumePartInDir_OrContinueOntoNextFullFileIfAllPartsOfCurrentFileHaveBeenPosted();
        return;
    }
    handleFullFilePostedToUsenet();
}
