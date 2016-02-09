#include "quickdirtystaticgitweb.h"

#include <QCoreApplication>
#include <QProcess>
#include <QFileInfo>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>

#include "lastmodifiedtimestamp.h"

#define GIT_BIN "git"
//yea I'm using libgit2 the git binary both. I just don't know how to do certain things in each one and don't care to learn. Dis QuickDirty after all
#define ANSI2HTML_BIN "/home/user/ansi2html.sh"
#define LAST_COMMIT_ID_WE_GENERATED_STATIC_GITWEB_FOR_SETTINGS_KEY "LastCommitIdWeGeneratedStaticGitwebFor"

//I was going to write a very similar converter to utilize jekyll's "blog" stuff, but wtf is the point I can just format the html myself and it gives me much more flexibility (I don't know ruby, and don't want to). text repo -> jeykll -> html, or just text repo -> html.... hmmmmmm. One huge thing I dislike about jekyll is that it doesn't have the TIME in the posts, only the date. Jekyll's blog generator stuff seems like a good idea for people who are new to git and making websites...... which is like 1% of github's community xD
//TODOoptional: we could use the input repo's commit IDs as tags in the destination output repo
//TODOmb: one hacky as fuck way to delete the diff htmls (once they are no longer linked to) is to sort a /diff/ dir by time, then delete all but the newest 100. some of those 100 may still be unreferenced, but no matter, they'd get deleted later. it's not that i don't know how to "properly" solve this problem, I just think it's not worth the effort. this doesn't work if I split up misc documents and software related documents
//TODOreq: handling deletes in the archive is kinda tricky (whereas handling deletes on the indexHtml (aka "most recent") is already done): If I delete an old file, I have to mark that the _month_ archive html which has a link to that [now deleted] file must be rebuilt (unless I use weekly/daily precision, which I don't intend on doing at this point). I should wait until I've handled all of the files additions/modifications/DELETIONS for a given commit before rebuilding that month archive html file. If after rebuilding it I realize it now has no entries at all, I delete it and then mark that the _year_ archive file, which has a link to that [now deleted] month archive, must be rebuilt. I should wait until I've handled/"rebuilt" (rebuilding includes "deleting" ofc) all of the month archive html files for a given year (or just all of the months of every year, no matter) before rebuilding that year archive html file. And of course, the same thing must be done for the year archive html file: if it becomes deleted because no more month entries, we then mark the "archive.html" as needing to be rebuilt in order to strip the link to that [now deleted] year archive html -- this is all pretty repetetive and I'm sure it can be done "intelligently" (so I could use the same logic for each level of precision), but the solution doesn't exactly jump out at me atm so I'll just code it manually and most likely facepalm at the end :)
//TODOmb: vanilla index.html files (aka "browse folders"). the one in the root should then be browse.html (since index.html in the root is my "most recent")
//TODOmb: embed svg
static int staticTreeWalkingCallback(const char *root, const git_tree_entry *entry, void *userData)
{
    QuickDirtyStaticGitWeb *quickDirtyStaticGitWeb = static_cast<QuickDirtyStaticGitWeb*>(userData);
    return quickDirtyStaticGitWeb->treeWalkingCallback(root, entry);
}
QuickDirtyStaticGitWeb::QuickDirtyStaticGitWeb(QObject *parent)
    : QObject(parent)
{
    QCoreApplication::setOrganizationName("QuickDirtyStaticGitWebOrganization");
    QCoreApplication::setOrganizationDomain("QuickDirtyStaticGitWebOrganizationDomain");
    QCoreApplication::setApplicationName("QuickDirtyStaticGitWeb");
}
int QuickDirtyStaticGitWeb::treeWalkingCallback(const char *root, const git_tree_entry *entry)
{
    git_otype objectType = git_tree_entry_type(entry);
    if(objectType != GIT_OBJ_BLOB)
        return 0;
    std::string rootStdString(root);
    QString filePath = appendSlashIfNeeded(QString::fromStdString(rootStdString));
    std::string entryNameStdString(git_tree_entry_name(entry));
    filePath.append(QString::fromStdString(entryNameStdString));
    filePath = removeLeadingSlashIfExisting(filePath);
    git_object *gitObject = NULL;
    if(git_tree_entry_to_object(&gitObject, m_InputSourceRepo, entry) != 0)
    {
        emitLastGitError();
        emit e("failed to get object from tree entry");
        m_TreeWalkError = true;
        return -1;
    }
    GitObjectFreeOnDestruct gitObjectFreeOnDestruct(gitObject);
    Q_UNUSED(gitObjectFreeOnDestruct)
    git_blob *gitBlob = (git_blob*)gitObject;
    const void *gitBlobRawContent = git_blob_rawcontent(gitBlob); //we don't own the data returned, and should not delete it! but let's cleverly wrap a qba around it to save time (and money)
    //FAIL (deep copy): QByteArray gitBlobBA(gitBlobRawContent, git_blob_rawsize(gitBlob));
    QByteArray gitBlobBA = QByteArray::fromRawData((const char*)gitBlobRawContent, git_blob_rawsize(gitBlob)); //success! the bytes were not copied and we now have a perpetual motion machine!
    QString fileHtml("<html><head><title>" + filePath.toHtmlEscaped() + "</title>" + cssStyleTag() + "</head>" + bodyHeader() + "<center><table><tr><td><pre>");

    {
        QString temp(gitBlobBA);
        fileHtml.append(temp.toHtmlEscaped());
    }
    fileHtml.append("</pre></td></tr></table></center>" + bodyFooter());

    QString fileNamePlusDotHtmlMostLikely = filePath;
    if(filePath == LAST_MODIFIED_TIMESTAMPS_FILEPATH)
    {
        fileHtml = QString(gitBlobBA);
    }
    else
    {
        fileNamePlusDotHtmlMostLikely += ".html";
    }

    //TODOreq: only make files that were added or modified [or renamed (and delete deletes and rename->oldFilename()s ofc)], otherwise we'd be rewriting the entire tree for every commit!
    if(m_HandlingRootCommit)
    {
        if((!ensureAllParentDirectoriesExistInTempPlaintextDirMakingThemIfNeeded(filePath)) || (!makeFileInTempPlaintextDirOverwritingIfNecessary(filePath, gitBlobBA)))
        {
            m_TreeWalkError = true;
            return -1;
        }
        if((!ensureAllParentDirectoriesExistMakingThemIfNeeded(filePath)) || (!makeFileInDestinationOutputStaticGitWebRepoOverwritingIfNecessary(fileNamePlusDotHtmlMostLikely, fileHtml)))
        {
            m_TreeWalkError = true;
            return -1;
        }
        m_TreeEntryPathsEitherAddedOrModified.append(removeLeadingSlashIfExisting(filePath));
    }
    else if(m_ModifiedEntriesLongerLived.removeOne(filePath) || m_AddedEntriesLongerLived.removeOne(filePath))
    {
        if((!ensureAllParentDirectoriesExistInTempPlaintextDirMakingThemIfNeeded(filePath)) || (!makeFileInTempPlaintextDirOverwritingIfNecessary(filePath, gitBlobBA)))
        {
            m_TreeWalkError = true;
            return -1;
        }
        if((!ensureAllParentDirectoriesExistMakingThemIfNeeded(filePath)) || (!makeFileInDestinationOutputStaticGitWebRepoOverwritingIfNecessary(fileNamePlusDotHtmlMostLikely, fileHtml)))
        {
            m_TreeWalkError = true;
            return -1;
        }
        m_TreeEntryPathsEitherAddedOrModified.append(removeLeadingSlashIfExisting(filePath));
    }
    return 0;
}
bool QuickDirtyStaticGitWeb::runAnsi2Html(const QByteArray &ansi2htmlInput)
{
    //QStringList ansi2HtmlArgs = QStringList() << "--palette=xterm" << "--bg=dark";
    m_Ansi2htmlProcess->start(ANSI2HTML_BIN/*, ansi2HtmlArgs*/);
    if(!m_Ansi2htmlProcess->waitForStarted(-1))
    {
        emit e("failed to start: " + QString(ANSI2HTML_BIN));
        return false;
    }
    m_Ansi2htmlProcess->write(ansi2htmlInput);
    m_Ansi2htmlProcess->closeWriteChannel();
    if(!m_Ansi2htmlProcess->waitForFinished(-1))
    {
        emit e("failed to finish: " + QString(ANSI2HTML_BIN));
        return false;
    }
    if((m_Ansi2htmlProcess->exitCode() != 0) || (m_Ansi2htmlProcess->exitStatus() != QProcess::NormalExit))
    {
        emit e("ansi2html exitted abnormally with exit code: " + QString::number(m_Ansi2htmlProcess->exitCode()));
        return false;
    }
    return true;
}
QPair<bool, QString> QuickDirtyStaticGitWeb::getHeadCommitId()
{
    QPair<bool, QString> ret;

    //ret.second = "c422693ee736cbf2635502aeaa911e9ce4ff27b6"; //hmm on second thought I really just need ANY commit-id in the log, since we're just climbing it to root. still, HEAD is the only one I know how to get dynamically atm

    //oh just found this piece of doc (i was gonna open the file myself xD):
//    git_oid oid = 0;
//    int error = git_reference_name_to_id(&oid, repo, "HEAD");

    git_oid headOid;
    if(git_reference_name_to_id(&headOid, m_InputSourceRepo, "HEAD") != 0)
    {
        emit e("failed to get HEAD commit id");
        ret.first = false;
    }
    else
    {
        ret.first = true;
        ret.second = gitOidToQString(&headOid);
    }
    return ret;
}
#define QUICK_DIRTY_STATIC_GIT_WEB_DATETIME_FORMAT_STRING "yyyy-MM-dd HH:mm:ss t"
QString QuickDirtyStaticGitWeb::createdFileHtml(const QString &htmlEncodedFilePath, qint64 fileTimestamp/*, const QString &unencodedFilePath*/)
{
    return QString();
    /*QString ret(QDateTime::fromMSecsSinceEpoch(fileTimestamp*1000).toString(QUICK_DIRTY_STATIC_GIT_WEB_DATETIME_FORMAT_STRING) + " -- d3fault created the file: <a href='" + htmlEncodedFilePath + "'>" + htmlEncodedFilePath + "</a><br />"); //<blockquote cite='" + htmlEncodedFilePath + "'>");

//    QFile file(m_DestinationOutputStaticGitWebRepo + unencodedFilePath);
//    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
//    {
//        emit e("error opening file for reading: " + unencodedFilePath);
//        return QString();
//    }
//    QTextStream textStream(&file);
//    ret.append(textStream.readAll().toHtmlEscaped());

//    ret.append("</blockquote>");
    return ret;*/
}
QString QuickDirtyStaticGitWeb::modifiedFileHtml(const QString &diffHtmlFilePath, const QString &htmlEncodedFilePath, qint64 modificationTimestamp)
{
//    QString htmlExt(".html");
//    QString actualFileName = htmlEncodedFilePath;
//    if(actualFileName.endsWith(htmlExt))
//        actualFileName.chop(htmlExt.length());
    return QString(QDateTime::fromMSecsSinceEpoch(modificationTimestamp*1000).toString(QUICK_DIRTY_STATIC_GIT_WEB_DATETIME_FORMAT_STRING) + " -- d3fault <a href='" + diffHtmlFilePath + "'>" A_STRING_IN_MODIFIED_FILE_HTML_KEK_HACK "='" + htmlEncodedFilePath + ".html'>" + htmlEncodedFilePath + "</a>");
}
bool QuickDirtyStaticGitWeb::ensureAllParentDirectoriesExistInTempPlaintextDirMakingThemIfNeeded(const QString &filePathOf_FILE_notDir)
{
    QFileInfo fileInfo(appendSlashIfNeeded(m_TempDirOfUnHtmlizedFileContentsOfFilesAddedOrModifiedDuringThisAppSession->path()) + filePathOf_FILE_notDir);
    QDir path = fileInfo.absoluteDir();
    return path.mkpath(path.path());
}
bool QuickDirtyStaticGitWeb::ensureAllParentDirectoriesExistMakingThemIfNeeded(const QString &filePathOf_FILE_notDir)
{
    QFileInfo fileInfo(m_DestinationOutputStaticGitWebRepo + filePathOf_FILE_notDir);
    QDir path = fileInfo.absoluteDir();
    return path.mkpath(path.path());
}
bool QuickDirtyStaticGitWeb::makeFileOverwritingIfNecessary(const QString &absoluteFilePath, const QString &fileContents)
{
    QFile newFile(absoluteFilePath);
    if(!newFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        emit e("failed to open file for writing: " + newFile.fileName());
        return false;
    }
    QTextStream newFileTextStream(&newFile);
    newFileTextStream << fileContents;
    newFileTextStream.flush();
    if(newFileTextStream.status() != QTextStream::Ok)
    {
        emit e("failed to write file via textstream (status=" + QString::number(newFileTextStream.status()) + "): " + newFile.fileName());
        return false;
    }
    if(!newFile.flush())
    {
        emit e("failed to flush file: " + newFile.fileName());
        return false;
    }
    return true;
}
bool QuickDirtyStaticGitWeb::makeFileInTempPlaintextDirOverwritingIfNecessary(const QString &relativeFilePath, const QString &fileContents)
{
    return makeFileOverwritingIfNecessary(appendSlashIfNeeded(m_TempDirOfUnHtmlizedFileContentsOfFilesAddedOrModifiedDuringThisAppSession->path()) + relativeFilePath, fileContents);
}
bool QuickDirtyStaticGitWeb::makeFileInDestinationOutputStaticGitWebRepoOverwritingIfNecessary(const QString &relativeFilePath, const QString &fileContents/*, const QDateTime &timestampToGiveTheFile*/)
{
    return makeFileOverwritingIfNecessary(m_DestinationOutputStaticGitWebRepo + relativeFilePath, fileContents);
}
bool QuickDirtyStaticGitWeb::makeFileInDestinationOutputStaticGitWebRepoIfItDoesntAlreadyExist(const QString &relativeFilePath, const QString &fileContents)
{
    QString newFilePath(m_DestinationOutputStaticGitWebRepo + relativeFilePath);
    if(QFile::exists(newFilePath))
    {
        emit e("file already exists: " + newFilePath);
        return false;
    }
    return makeFileInDestinationOutputStaticGitWebRepoOverwritingIfNecessary(relativeFilePath, fileContents);
}
bool QuickDirtyStaticGitWeb::rewriteStaleArchiveHtmls(ArchiveHtmls *archiveHtmls)
{
    //TODOreq: we only show "mutatons" from the current app session (which, when starting from the root commit, does give us every file), but we need to show all files for a given month, including some that were "mutated" (added) in previous app sessions. The same "bug" applies to months (on years archive html) and years (on archive html) I am guessing (but am unsure and can't wrap my head around it). The mutations were needed to detect which files became stale, but are not enough to rewrite them entirely
    //^eh basically the mutations are worthless (i don't iterate them to rewrite stale archive htmls like I'm currently erroneously doing) except to detect staleness

    //QSet<ArchiveMonthHtml*> staleMonths;
    Q_FOREACH(ArchiveMonthHtml *currentStaleMonth, archiveHtmls->staleMonths())
    {
        //TODOreq: during this foreach is when staleYears becomes populated via deletes (it also gets populated on new adds/modifications (handled earlier) ofc). it's when we finally notice that certain months no longer have entries
        QString archiveMonthHtmlFileName("archive-" + QString::number(currentStaleMonth->parentArchiveYear()->year()) + "-" + QDate::longMonthName(currentStaleMonth->month()).toLower() + ".html");
        //QMultiMap<ARCHIVE_MONTH_HTML_ENTRY_MAP_TYPES> currentStaleMonthEntries = currentStaleMonth->entries();
        QMultiMap<ARCHIVE_MONTH_HTML_ENTRY_MAP_TYPES> currentStaleMonthEntries = getAllEntriesForThisMonthNotJustMutatedInThisAppSession(currentStaleMonth->parentArchiveYear()->year(), currentStaleMonth->month()); //FFFFFFFFFFFFFFFUCK getting the commit time from files mutated in previous app sessions is going to be a bitch (idk how to even do it!!), whereas getting the lastModifiedTimestamp is ez pz :). we can't rely on lastModifiedTimestamps file to exist though and fallback to commit time fuck fuck fuck fuck how i do it. one HACKY way is to `touch` the files when we create them [in previous app sessions] (with either commit time or lastModifiedTimestamp just to keep shit simple here/now), then to simply read in that timestamp. Fuck yea that's definitely the easiest, but perhaps not the most efficient. Fuck it at this point. QDirIterator + fileInfo.lastModified it is. We need to ignore our own generated files (index.html, diff/*, archive*.html, about.html, and .git) during that QDirIteration. TODOreq
        if(currentStaleMonthEntries.isEmpty())
        {
            //TO DOnereq: delete the file. delete the ArchiveMonthHtml [scoped] pointer in the year's list. mark that year as stale
            if(!QFile::remove(m_DestinationOutputStaticGitWebRepo + archiveMonthHtmlFileName))
            {
                emit e("failed to delete file: " + archiveMonthHtmlFileName);
                //return false;
            }
            currentStaleMonth->parentArchiveYear()->removeMonth(currentStaleMonth);
        }
        else
        {
            //truncate and recreate the ArchiveMonthHtml
            /*QFile archiveMonthHtmlFile(archiveMonthHtmlFileName);
            if(!archiveMonthHtmlFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
            {
                //emit e("failed to open file for writing: " + archiveMonthHtmlFileName);
                return;
            }*/
            QString archiveMonthHtmlFileContents;
            QTextStream archiveMonthHtmlFileTextStream(&archiveMonthHtmlFileContents);
            archiveMonthHtmlFileTextStream << "<html><head><title>Archive - " << QString::number(currentStaleMonth->parentArchiveYear()->year()) << " - " << QDate::longMonthName(currentStaleMonth->month()) << "</title>" + cssStyleTag() + "</head>";
            archiveMonthHtmlFileTextStream << bodyHeader();
            QMapIterator<ARCHIVE_MONTH_HTML_ENTRY_MAP_TYPES> monthEntriesIterator(currentStaleMonthEntries);
            monthEntriesIterator.toBack();
            archiveMonthHtmlFileTextStream << "<center><table>";
            bool even = true;
            while(monthEntriesIterator.hasPrevious())
            {
                monthEntriesIterator.previous();
                QString htmlEscapedPath = monthEntriesIterator.value().toHtmlEscaped();
                QString actualFilename = htmlEscapedPath;
                QString htmlExt(".html");
                if(actualFilename.endsWith(".html"))
                    actualFilename.chop(htmlExt.length());
                archiveMonthHtmlFileTextStream << "<tr class='" + QString(even ? "d0" : "d1") + "'><td>" << QDateTime::fromMSecsSinceEpoch(monthEntriesIterator.key() * 1000).toString(QUICK_DIRTY_STATIC_GIT_WEB_DATETIME_FORMAT_STRING) << " -- " << "<a href='" << htmlEscapedPath << "'>" << actualFilename << "</a></td></tr>\n";
                even = !even;
            }
            archiveMonthHtmlFileTextStream << "</table></center>";
            archiveMonthHtmlFileTextStream << bodyFooter();
            archiveMonthHtmlFileTextStream.flush();
            /*if(!archiveMonthHtmlFile.flush())
            {
                //emi
                //blah these are clearly data types and should not have methods such action-esq methods on them ("add + mark [self+parent] as stale helper methods" are fine ofc). too lazy to refactor atm -- ok it was necessary since I needed m_DestinationOutputStaticGitWebRepo
            }*/
            if(!makeFileInDestinationOutputStaticGitWebRepoOverwritingIfNecessary(archiveMonthHtmlFileName, archiveMonthHtmlFileContents))
                return false;
        }
    }

    Q_FOREACH(ArchiveYearHtml *currentStaleYear, archiveHtmls->staleYears())
    {
        QString archiveYearHtmlFileName("archive-" + QString::number(currentStaleYear->year()) + ".html");
        //QMap<MONTHS_ARCHIVE_HTMLS_MAP_TYPES > months = currentStaleYear->monthEntries();
        QMap<int /*month*/, QString /*path*/ > months = getAllMonthsWithEntriesForThisYearNotJustMutatedInThisAppSession(currentStaleYear->year());
        if(months.isEmpty())
        {
            //TODOreq: delete the file. delete the ArchiveYearHtml [scoped] pointer in the archive's list. mark the archive as stale
            if(!QFile::remove(m_DestinationOutputStaticGitWebRepo + archiveYearHtmlFileName))
            {
                emit e("failed to delete file: " + archiveYearHtmlFileName);
                //return false;
            }
            currentStaleYear->parentArchiveHtmls()->removeYear(currentStaleYear);
        }
        else
        {
            //truncate and recreate the ArchiveYearHtml
            QString archiveYearHtmlFileContents;
            QTextStream archiveYearHtmlTextSteam(&archiveYearHtmlFileContents);
            archiveYearHtmlTextSteam << "<html><head><title>Archive - " << QString::number(currentStaleYear->year()) << "</title>" + cssStyleTag() + "</head>" << bodyHeader();
            QMapIterator<int, QString> monthsIterator(months);
            monthsIterator.toBack();
            archiveYearHtmlTextSteam << "<center><table>";
            bool even = true;
            while(monthsIterator.hasPrevious())
            {
                monthsIterator.previous();
                //QString htmlEscapedYear
                QString  archiveMonthHtmlFilePath = "archive-" + QString::number(currentStaleYear->year()) + "-" + QDate::longMonthName(monthsIterator.key()).toLower() + ".html";
                archiveYearHtmlTextSteam << "<tr class='" + QString(even ? "d0" : "d1") + "'><td><a href='" << archiveMonthHtmlFilePath << "'>" << QDate::longMonthName(monthsIterator.key()) << "</a></td></tr>\n";
                even = !even;
            }
            archiveYearHtmlTextSteam << "</table></center>";
            archiveYearHtmlTextSteam << bodyFooter();
            archiveYearHtmlTextSteam.flush();
            if(!makeFileInDestinationOutputStaticGitWebRepoOverwritingIfNecessary(archiveYearHtmlFileName, archiveYearHtmlFileContents))
                return false;
        }
    }

//    if(archiveHtmls->isStale())
//    {
//        QString archiveHtmlFileName = "archive.html";
//        //QMap<YEARS_ARCHIVE_HTMLS_TYPES > years = archiveHtmls->yearsArchiveHtmls();
//        QMap<int, QString> years = getAllYearsWithMonthEntriesNotJustMutatedInThisAppSession();
//        if(years.isEmpty())
//        {
//            //lol wut you deleted everything in your repo? umm k
//            if(!QFile::remove(m_DestinationOutputStaticGitWebRepo + archiveHtmlFileName))
//            {
//                emit e("failed to delete file: " + archiveHtmlFileName);
//                //return false;
//            }
//        }
//        else
//        {
//            //truncate and recreate archive.html
//            QString archiveHtmlFileContents;
//            QTextStream archiveHtmlTextStream(&archiveHtmlFileContents);
//            archiveHtmlTextStream << "<html><head><title>Archive</title>" + cssStyleTag() + "</head>" << bodyHeader();
//            QMapIterator<int, QString> yearsIterator(years);
//            yearsIterator.toBack();
//            archiveHtmlTextStream << "<center><table>";
//            bool even = true;
//            while(yearsIterator.hasPrevious())
//            {
//                yearsIterator.previous();
//                QString archiveYearHtmlFilePath = "archive-" + QString::number(yearsIterator.key()) + ".html";
//                archiveHtmlTextStream << "<tr class='" + QString(even ? "d0" : "d1") + "'><td><a href='" << archiveYearHtmlFilePath << "'>" << QString::number(yearsIterator.key()) << "</a></td></tr>\n";
//                even = !even;
//            }
//            archiveHtmlTextStream << "</table></center>";
//            archiveHtmlTextStream << bodyFooter();
//            archiveHtmlTextStream.flush();
//            if(!makeFileInDestinationOutputStaticGitWebRepoOverwritingIfNecessary(archiveHtmlFileName, archiveHtmlFileContents))
//                return false;
//        }
//    }
    QString archiveHtmlFileName = "archive.html";
    QMap<int /*year*/, QMap<int /*month*/, bool /*unused-lazisorter*/> > allMonths = getAllMonthsArchiveHtmls();
    //truncate and recreate archive.html
    QString archiveHtmlFileContents;
    QTextStream archiveHtmlTextStream(&archiveHtmlFileContents);
    archiveHtmlTextStream << "<html><head><title>Archive</title>" + cssStyleTag() + "</head>" << bodyHeader();
    QMapIterator<int, QMap<int /*month*/, bool /*unused-lazisorter*/> > allMonthsIterator(allMonths);
    allMonthsIterator.toBack();
    archiveHtmlTextStream << "<center><table>";
    bool even = true;
    while(allMonthsIterator.hasPrevious())
    {
        allMonthsIterator.previous();
        int year = allMonthsIterator.key();
        QMapIterator<int /*month*/, bool> monthsInAgivenYearIterator = allMonthsIterator.value();
        monthsInAgivenYearIterator.toBack();
        while(monthsInAgivenYearIterator.hasPrevious())
        {
            monthsInAgivenYearIterator.previous();
            int month = monthsInAgivenYearIterator.key();
            QString archiveMonthFilePath = "archive-" + QString::number(year) + "-" + QDate::longMonthName(month).toLower() + ".html";
            archiveHtmlTextStream << "<tr class='" + QString(even ? "d0" : "d1") + "'><td><a href='" << archiveMonthFilePath << "'>" << QString::number(year) << " - " << QDate::longMonthName(month) << "</a></td></tr>\n";
            even = !even;
        }
    }
    archiveHtmlTextStream << "</table></center>";
    archiveHtmlTextStream << bodyFooter();
    archiveHtmlTextStream.flush();
    if(!makeFileInDestinationOutputStaticGitWebRepoOverwritingIfNecessary(archiveHtmlFileName, archiveHtmlFileContents))
        return false;


    archiveHtmls->clearStaleMonths();
    archiveHtmls->clearStaleYears();
    archiveHtmls->setStale(false);

    return true;
}
bool QuickDirtyStaticGitWeb::touchFileInDestinationDir(const QString &relativePath, qint64 timestamp)
{
    //give the file it's timestamp. TODOportability guh I should add QFileInfo::setLastModified once and for all. or even having it be specifiable during QFile::open(Write...
    QStringList touchArgs = QStringList() << QString("--date=@" + QString::number(timestamp)) << QString(m_DestinationOutputStaticGitWebRepo + relativePath + ".html");
    QProcess touchProcess;
    touchProcess.start("touch", touchArgs);
    if(!touchProcess.waitForStarted(-1))
    {
        emit e("failed to start: touch");
        return false;
    }
    if(!touchProcess.waitForFinished(-1))
    {
        emit e("failed to finish: touch");
        return false;
    }
    if((touchProcess.exitCode() != 0) || (touchProcess.exitStatus() != QProcess::NormalExit))
    {
        emit e("touch exitted abnormally with exit code: " + QString::number(touchProcess.exitCode()));
        return false;
    }
    return true;
}
QMultiMap<qint64, QString> QuickDirtyStaticGitWeb::getAllEntriesForThisMonthNotJustMutatedInThisAppSession(int year, int month) const
{
    QMultiMap<qint64, QString> ret;

    QDirIterator dirIt(m_DestinationOutputStaticGitWebRepo, QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden, QDirIterator::Subdirectories);
    while(dirIt.hasNext())
    {
        dirIt.next();
        QFileInfo fileInfo = dirIt.fileInfo();
        if(fileInfo.isDir())
            continue;

        QDateTime dateTime = fileInfo.lastModified();
        if(dateTime.date().year() != year)
            continue;
        if(dateTime.date().month() != month)
            continue;

        QString filePath = fileInfo.absoluteFilePath();
        if(filePath.startsWith(m_DestinationOutputStaticGitWebRepo + ".git/"))
            continue;
        if(filePath.startsWith(m_DestinationOutputStaticGitWebRepo + "diff/"))
            continue;
        if(filePath.startsWith(m_DestinationOutputStaticGitWebRepo + "archive-") && filePath.endsWith(".html"))
            continue;
        if(filePath.startsWith(m_DestinationOutputStaticGitWebRepo + "sitemap-") && filePath.endsWith(".xml"))
            continue;

        QStringList excludeFilesInRootOfDestinationRepo = QStringList() << "index.html" << "archive.html" << "about.html" << "CNAME" << "horsesAt1446072828.png" << "horsesAt1446072828.thumbnail.png" << "tentAt1447882022.png" << "tentAt1447882022.thumbnail.png" << "robots.txt" << "sitemap.xml" << LAST_MODIFIED_TIMESTAMPS_FILEPATH; //TODOreq: exclude CNAME and .git from the sitemap generation
        QStringList excludeFilePaths;
        Q_FOREACH(QString bleh, excludeFilesInRootOfDestinationRepo)
        {
            excludeFilePaths << m_DestinationOutputStaticGitWebRepo + bleh;
        }
        if(excludeFilePaths.contains(filePath))
            continue;

        QString relativePath = filePath;
        relativePath.remove(0, m_DestinationOutputStaticGitWebRepo.length());
        ret.insert(dateTime.toMSecsSinceEpoch() / 1000, relativePath);
    }

    return ret;
}
QMap<int, QString> QuickDirtyStaticGitWeb::getAllMonthsWithEntriesForThisYearNotJustMutatedInThisAppSession(int year)
{
    QMap<int, QString> ret;

    QDir destDir(m_DestinationOutputStaticGitWebRepo);
    QStringList allMonthsArchivesForThatYear = destDir.entryList(QStringList() << QString("archive-" + QString::number(year) + "-*.html"), QDir::Files);
    Q_FOREACH(const QString &currentMonthArchiveFile, allMonthsArchivesForThatYear)
    {
        QStringList fileNameSplitAtHyphens = currentMonthArchiveFile.split("-");
        if(fileNameSplitAtHyphens.size() != 3)
        {
            emit e("eh split at hyphens wasn't 3. I probably globbed wrong: " + currentMonthArchiveFile);
            continue;
        }
        QString monthPartOfFilename = fileNameSplitAtHyphens.last();
        QString htmlExt(".html");
        if(monthPartOfFilename.endsWith(htmlExt))
            monthPartOfFilename.chop(htmlExt.length());
        int month = QDate::fromString(monthPartOfFilename, "MMMM").month();
        ret.insert(month, currentMonthArchiveFile);
    }

    return ret;
}
QMap<int, QString> QuickDirtyStaticGitWeb::getAllYearsWithMonthEntriesNotJustMutatedInThisAppSession()
{
#if 0 //num hyphens ezier
    QStringList monthExcludeLol;
    for(int i = 1; i < 12+1; ++i)
    {
        monthExcludeLol << QDate::longMonthName(i).toLower();
    }
#endif

    QMap<int, QString> ret;
    QDir destDir(m_DestinationOutputStaticGitWebRepo);
    QStringList allYearAndMonthArchivesInDest = destDir.entryList(QStringList() << "archive-*.html", QDir::Files);
    Q_FOREACH(const QString &currentMonthOrYearArchiveFileName, allYearAndMonthArchivesInDest)
    {
        QStringList fileNameSplitAtHyphens = currentMonthOrYearArchiveFileName.split("-");
        if(fileNameSplitAtHyphens.size() != 2)
            continue; //month archive file most likely, skip it
        QString yearPartOfFilename = fileNameSplitAtHyphens.last();
        QString htmlExt(".html");
        if(yearPartOfFilename.endsWith(htmlExt))
            yearPartOfFilename.chop(htmlExt.length());
        bool convertOk = false;
        int year = yearPartOfFilename.toInt(&convertOk);
        if(!convertOk)
        {
            emit e("failed to parse year out of this filename: " + currentMonthOrYearArchiveFileName);
            continue;
        }
        ret.insert(year, currentMonthOrYearArchiveFileName);
    }

    return ret;
}
QMap<int, QMap<int, bool> > QuickDirtyStaticGitWeb::getAllMonthsArchiveHtmls()
{
    QMap<int, QMap<int, bool> > ret;
    QDir destDir(m_DestinationOutputStaticGitWebRepo);
    QStringList allYearAndMonthArchivesInDest = destDir.entryList(QStringList() << "archive-*.html", QDir::Files);
    Q_FOREACH(const QString &currentMonthOrYearArchiveFileName, allYearAndMonthArchivesInDest)
    {
        QStringList fileNameSplitAtHyphens = currentMonthOrYearArchiveFileName.split("-");
        if(fileNameSplitAtHyphens.size() != 3)
            continue; //[old, no longer in use] year archive file most likely, skip it

        QString monthPartOfFilename = fileNameSplitAtHyphens.last();
        QString htmlExt(".html");
        if(monthPartOfFilename.endsWith(htmlExt))
            monthPartOfFilename.chop(htmlExt.length());
        int month = QDate::fromString(monthPartOfFilename, "MMMM").month();

        QString yearPartOfFilename = fileNameSplitAtHyphens.at(1);
        bool convertOk = false;
        int year = yearPartOfFilename.toInt(&convertOk);
        if(!convertOk)
        {
            emit e("failed to parse year out of this filename: " + currentMonthOrYearArchiveFileName);
            continue;
        }

        QMap<int /*month*/, bool> months;
        if(ret.contains(year))
            months = ret.value(year);
        months.insert(month, true);
        ret.insert(year, months);
    }

    return ret;
}
//TODOreq: CNAME file for jekyll
void QuickDirtyStaticGitWeb::makeStaticAboutFileIfItNeedsToChange()
{
    QString aboutFileContents = "<html><head><title>Solitude</title></head>" + bodyHeader() + "<center><a href='tentAt1447882022.png'><img src='tentAt1447882022.thumbnail.png' alt='tentAt1447882022'></a><br /><br /><a href='horsesAt1446072828.png'><img src='horsesAt1446072828.thumbnail.png' alt='horsesAt1446072828'></a><br /></center>" + bodyFooter();
    QString aboutFileName = m_DestinationOutputStaticGitWebRepo + "about.html";
    QFile aboutFile(aboutFileName);
    if(aboutFile.exists())
    {
        if(!aboutFile.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream textStream(&aboutFile);
        QString writtenFileContents = textStream.readAll();
        if(writtenFileContents == aboutFileContents)
            return;
    }
    aboutFile.close();
    if(!aboutFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return;
    QTextStream textStream(&aboutFile);
    textStream << aboutFileContents;
}
void QuickDirtyStaticGitWeb::makeStaticCNAMEIfItDoesntExist()
{
    QFile cName(m_DestinationOutputStaticGitWebRepo + "CNAME");
    if(cName.exists() || (!cName.open(QIODevice::WriteOnly | QIODevice::Text)))
        return;
    QTextStream textStream(&cName);
    textStream << "d3fault.net";
}
void QuickDirtyStaticGitWeb::makeStaticRobotsIfItDoesntExist()
{
    QFile robots(m_DestinationOutputStaticGitWebRepo + "robots.txt");
    if(robots.exists() || (!robots.open(QIODevice::WriteOnly | QIODevice::Text)))
        return;
    QTextStream textStream(&robots);
    textStream << "http://d3fault.net/sitemap.xml";
}
QString QuickDirtyStaticGitWeb::cssStyleTag()
{
    QString ret("<style>\n");
    ret.append("tr.d0 td { background-color: lightGray; color: black; }\n");
    ret.append("tr.d1 td { background-color: white; color: black; }\n");
    ret.append("pre { white-space: pre-wrap;       /* CSS 3 */");
    ret.append("white-space: -moz-pre-wrap;  /* Mozilla, since 1999 */");
    ret.append("white-space: -pre-wrap;      /* Opera 4-6 */");
    ret.append("white-space: -o-pre-wrap;    /* Opera 7 */");
    ret.append("word-wrap: break-word;       /* Internet Explorer 5.5+ */");
    ret.append("}");
    ret.append("</style>");
    return ret;
}
QString QuickDirtyStaticGitWeb::bodyHeader()
{
    QString ret("<body>\n<center><a href='/index.html'>Home</a> <a href='/archive.html'>Archive</a> <a href='https://github.com/d3faultdotxbe/autobackup'>Code</a> <a href='/about.html'>About</a></center>\n<br />\n");
    return ret;
}
QString QuickDirtyStaticGitWeb::bodyFooter()
{
    QString ret("<br />\n<center>Download my brain: <a href='https://github.com/d3faultdotxbe/binaryMeta/raw/master/d3faults.brain.torrent'>Torrent</a><br />\n");
    ret.append("Bitcoin: 1FwZENuqEHHNCAz4fiWbJWSknV4BhWLuYm</center></body></html>");
    return ret;
}
QString QuickDirtyStaticGitWeb::gitOidToQString(const git_oid *oid)
{
    char oidCharArray[GIT_OID_HEXSZ+1];
    if(!git_oid_tostr(&oidCharArray[0], GIT_OID_HEXSZ+1, oid))
    {
        emitLastGitError();
        emit e("failed to convert oid to char array");
        return QString();
    }
    std::string oidStdString(oidCharArray, GIT_OID_HEXSZ/*+1 <- we don't need the null character anymore*/);
    QString oidQString = QString::fromStdString(oidStdString);
    return oidQString;
}
bool QuickDirtyStaticGitWeb::runGit(QStringList gitArgs)
{
    m_GitProcess->start(GIT_BIN, gitArgs);
    if(!m_GitProcess->waitForStarted(-1))
    {
        emit e("failed to start: " + QString(GIT_BIN) + " " + gitArgs.join(" "));
        return false;
    }
    if(!m_GitProcess->waitForFinished(-1))
    {
        emit e("failed to finish: " + QString(GIT_BIN) + " " + gitArgs.join(" "));
        return false;
    }
    if((m_GitProcess->exitCode() != 0) || (m_GitProcess->exitStatus() != QProcess::NormalExit))
    {
        emit e("git exitted abnormally with exit code: " + QString::number(m_GitProcess->exitCode()));
        return false;
    }
    return true;
}
void QuickDirtyStaticGitWeb::emitLastGitError()
{
    const git_error *gitError = giterr_last();
    if(gitError)
    {
        QString gitErrorQString(gitError->message);
        emit e(gitErrorQString);
    }
}
void QuickDirtyStaticGitWeb::generateStaticGitWeb(const QString &sourceInputRepoFilePath, const QString &destinationOutputStaticGitWebRepo)
{
    ScopedQuickDirtyStaticGitWebResultEmitterTypedef resultEmitter(new QuickDirtyStaticGitWebResultEmitter());
    connect(resultEmitter.data(), SIGNAL(finishedGeneratingStaticGitWeb(bool)), this, SIGNAL(finishedGeneratingStaticGitWeb(bool))); //exceptions? HAH!

    QFileInfo sourceInputRepoFileInfo(sourceInputRepoFilePath);
    if(!sourceInputRepoFileInfo.isDir())
    {
        emit e("input is not a dir or doesn't exist: " + sourceInputRepoFilePath);
        return;
    }
    QFileInfo destinationOutputStaticGitWebRepoFileInfo(destinationOutputStaticGitWebRepo);
    if(!destinationOutputStaticGitWebRepoFileInfo.isDir())
    {
        emit e("output is not a dir or doesn't exist: " + destinationOutputStaticGitWebRepo);
        return;
    }
    m_DestinationOutputStaticGitWebRepo = appendSlashIfNeeded(destinationOutputStaticGitWebRepo);

    //    //git log --reverse
    //    m_GitProcess = new QProcess(this);
    //    m_GitProcess->setWorkingDirectory(sourceInputRepo);
    //    QStringList gitLogArgs = QStringList() << "log" << "--reverse" << "--format=oneline";
    //    if(!runGit(gitLogArgs))
    //        return;

    //    //parse the commit-ids out of the output
    //    QTextStream gitProcessTextStream(m_GitProcess);

    //    QStringList commitIds;
    //    while(!gitProcessTextStream.atEnd())
    //    {
    //        QString currentLine = gitProcessTextStream.readLine();
    //        if(currentLine.trimmed().isEmpty())
    //            continue;
    //        commitIds << currentLine.split(" ").at(0); //TODOmb: sanitize at(0) first
    //    }

    //    QString rootCommitId = commitIds.first();

    //    {
    //        QTemporaryDir tempDirForCheckingOutRootCommit(rootCommmitId);
    //        if(!tempDirForCheckingOutRootCommit.isValid())
    //        {
    //            emit e("failed to create temp dir: " + tempDirForCheckingOutRootCommit.path());
    //            return;
    //        }
    //        QStringList gitCloneArgs = QStringList() << "clone" << sourceInputRepo << tempDirForCheckingOutRootCommit.path();
    //        if(!runGit(gitCloneArgs))
    //            return;
    //        QMap<QDateTime, QStringList> allPathsSortedByDateTime;
    //        QString lastModifiedTimestampsFilePath = appendSlashIfNeeded(tempDirForCheckingOutRootCommit.path() + LAST_MODIFIED_TIMESTAMPS_FILEPATH);
    //        if(!QFile::exists(lastModifiedTimestampsFilePath))
    //        {

    //        }
    //    }

    QSettings settings;
    QString lastCommitIdWeGeneratedStaticGitwebFor_OrEmptyStringIfFirstRun = settings.value(LAST_COMMIT_ID_WE_GENERATED_STATIC_GITWEB_FOR_SETTINGS_KEY).toString(); //TODOmb: 'profiles', where the input dir string is they "key". maybe though the "origin url" should be the "key", to allow for moving the input src dir around. the output dir could also be a tempdir that is NOT deleted and also saved as a value.
    m_HandlingRootCommit = lastCommitIdWeGeneratedStaticGitwebFor_OrEmptyStringIfFirstRun.isEmpty();

    if(git_threads_init() != 0)
    {
        emitLastGitError();
        emit e("failed to init git threads");
        return;
    }
    GitThreadsShutdownOnDestruct gitThreadsShutdownOnDestruct;
    Q_UNUSED(gitThreadsShutdownOnDestruct)

    const std::string sourceInputRepoFilePathStdString = sourceInputRepoFilePath.toStdString();
    m_InputSourceRepo = NULL;
    if(git_repository_open(&m_InputSourceRepo, sourceInputRepoFilePathStdString.c_str()) != 0)
    {
        emitLastGitError();
        emit e("failed to git_repository_open: " + sourceInputRepoFilePath);
        return;
    }
    GitRepositoryFreeOnDestruct gitRepositoryFreeOnDestruct(m_InputSourceRepo);
    Q_UNUSED(gitRepositoryFreeOnDestruct)

    //git_commit *wcommit;
    git_oid oid;

#if 0 //my brain!!!
    if(m_HandlingRootCommit)
    {
        //determine root commit-id by revwalking backwards (think: michael jackson)

        git_revwalk *revWalk = NULL;
        git_revwalk_new(&revWalk, m_InputSourceRepo);
        GitRevwalkFreeOnDestruct gitRevwalkFreeOnDestruct(revWalk);
        Q_UNUSED(gitRevwalkFreeOnDestruct)

//        QPair<bool, QString> headCommitId = getHeadCommitId(sourceInputRepoFilePath);
//        if(!headCommitId.first)
//            return;
//        const std::string headCommitIdStdString = headCommitId.second.toStdString();
//        git_oid_fromstr(&oid, headCommitIdStdString.c_str());
        git_revwalk_sorting(revWalk, GIT_SORT_TOPOLOGICAL | GIT_SORT_TIME);
//        git_revwalk_push(revWalk, &oid);
        if(git_revwalk_push_head(revWalk) != 0)
        {
            emitLastGitError();
            emit e("failed to git_revwalk_push_head");
            return;
        }

        //time_t ctime;

        while((git_revwalk_next(&oid, revWalk)) == 0)
        {
//            if(git_commit_lookup(&wcommit, sourceInputRepo, &oid) != 0)
//            {
//                emit e("error during git_commit_lookup in revwalk");
//                return;
//            }
//            ctime = git_commit_time(wcommit);
//            git_commit_free(wcommit);
        }

        //even though it looks like we did nothing, the oid now points to the root/first commit
    }
    else
    {
        //pick up where we left off last time the app was run. revwalk forward one (unless no more child commits)

        git_revwalk *revWalk = NULL;
        git_revwalk_new(&revWalk, m_InputSourceRepo);
        GitRevwalkFreeOnDestruct gitRevwalkFreeOnDestruct(revWalk);
        Q_UNUSED(gitRevwalkFreeOnDestruct)

        const std::string lastCommitIdWeGeneratedStaticGitwebForStdString = lastCommitIdWeGeneratedStaticGitwebFor_OrEmptyStringIfFirstRun.toStdString();
        git_oid_fromstr(&oid, lastCommitIdWeGeneratedStaticGitwebForStdString.c_str());
        git_revwalk_sorting(revWalk, GIT_SORT_TOPOLOGICAL | GIT_SORT_TIME);
        //git_revwalk_push(revWalk, &oid);
        QString rangeQString = lastCommitIdWeGeneratedStaticGitwebFor_OrEmptyStringIfFirstRun;
        rangeQString.append("..HEAD");
        std::string rangeStdString = rangeQString.toStdString();
        git_revwalk_push_range(revWalk, rangeStdString.c_str());
        if(git_revwalk_next(&oid, revWalk) != 0)
        {
            //TODOmb: detect "no child commits" by comparing HEAD (got from libgit) to lastCommitIdWeGeneratedStaticGitwebFor_OrEmptyStringIfFirstRun, ez pz
            //no children commits, this app shouldn't have been called (and it wouldn't have been if properly used via post-update as planned). just quit
            return;
        }
        //oid now points at the first unprocessed commit
    }
#endif

    QPair<bool, QString> headComitId = getHeadCommitId();
    if(!headComitId.first)
        return; //no commits yet, most likely
    if(headComitId.second == lastCommitIdWeGeneratedStaticGitwebFor_OrEmptyStringIfFirstRun)
    {
        emit e("no children commits to process since last run");
        return; //no children commits, this app shouldn't have been called (and it wouldn't have been if properly used via post-update as planned). just quit
    }

    //revwalk forward from wherever, doing git show --color-words, until no more children commits seen
    git_revwalk *revWalk = NULL;
    git_revwalk_new(&revWalk, m_InputSourceRepo);
    GitRevwalkFreeOnDestruct gitRevwalkFreeOnDestruct(revWalk);
    Q_UNUSED(gitRevwalkFreeOnDestruct)
    git_revwalk_sorting(revWalk, GIT_SORT_TOPOLOGICAL | GIT_SORT_TIME | GIT_SORT_REVERSE);
    if(m_HandlingRootCommit)
    {
        emit e("starting from root commit");
        git_revwalk_push_head(revWalk);
    }
    else
    {
        emit e("resuming from: " + lastCommitIdWeGeneratedStaticGitwebFor_OrEmptyStringIfFirstRun);
        //git_revwalk_push(revWalk, &oid);
        QString rangeQString(lastCommitIdWeGeneratedStaticGitwebFor_OrEmptyStringIfFirstRun + "..HEAD");
        std::string rangeStdString = rangeQString.toStdString();
        git_revwalk_push_range(revWalk, rangeStdString.c_str());
    }

    git_oid parentOid; //invalid when handlingRootCommit == true
    GitTreeFreeOnDestruct gitParentTreeFreeOnDestruct;
    GitTreeFreeOnDestruct gitCurrentTreeFreeOnDestruct;
    QString diffHtmlFilePath;

    IndexHtmlFifo indexHtmlFifo; //TODOreq: QSettings cache all (100 when full) the entries in this indexHtmlFifo just before application exits. rationale: if we didn't, when "resuming" (new app session) from a particular commit-id, we'd have to reparse N previous commit-IDs (as many as it takes to fill up 100) before even considering going onto the next commit. TODOreq: the "show limit" should still be 100, but by keeping more, say 150-200, I can re-use old entries whenever a "delete" occurs (without such buffer, I'd either then have 99 (or less) unless I re-parsed backwards. Just so much easier to store the "last 100/200" in a QSettings each time. //TODOreq: delete the "modification diff html files" when all the files that refer to them on the indexHtml (aka "most recent") disappear. this however fucks with the previous "keep 200 unseen cache" thingo to handle deletes, as the html modified diff would have to be recretaed (unless we kept it until it was pushed out of all _200_ instead of _100_ (nothing would link to it for a period of time, but o well))
    //indexHtmlFifo.loadCacheFromSettings();
    ArchiveHtmls archiveHtmls;

    GitCommitFreeOnDestruct parentGitCommit; //we need to know this so we can look up the parent commit's timestamp when there's a delete and no last modified timestamps file there to give us the timestamp of the now-deleted file. HOW ELSE am I supposed to know which archive month html file needs to be rebuilt. fuck my brain hurts

    makeStaticAboutFileIfItNeedsToChange();
    makeStaticCNAMEIfItDoesntExist();
    //makeStaticRobotsIfItDoesntExist();

    m_TempDirOfUnHtmlizedFileContentsOfFilesAddedOrModifiedDuringThisAppSession.reset(new QTemporaryDir());
    if(!m_TempDirOfUnHtmlizedFileContentsOfFilesAddedOrModifiedDuringThisAppSession->isValid())
    {
        emit e("failed to create temp dir");
        return;
    }

    int numCommitsProcessed = 0;
    while(git_revwalk_next(&oid, revWalk) == 0)
    {
        QString oidQString = gitOidToQString(&oid);
        git_commit *commit = NULL;
        if(git_commit_lookup(&commit, m_InputSourceRepo, &oid) != 0)
        {
            emitLastGitError();
            emit e("failed to look up commit: " + oidQString);
            return;
        }
        GitCommitFreeOnDestruct gitCommitFreeOnDestruct(commit);
        Q_UNUSED(gitCommitFreeOnDestruct)
        git_tree *tree = NULL;
        if(git_commit_tree(&tree, commit) != 0)
        {
            emitLastGitError();
            emit e("error looking up commit's tree: " + oidQString);
            return;
        }
        if(!gitCurrentTreeFreeOnDestruct.isNull())
        {
            gitParentTreeFreeOnDestruct.reset(gitCurrentTreeFreeOnDestruct.take());
        }
        gitCurrentTreeFreeOnDestruct.reset(tree);

        QStringList addedEntries, modifiedEntries;

        if(!m_HandlingRootCommit)
        {
            if(gitParentTreeFreeOnDestruct.isNull())
            {
                //special case: load the tree of the commit we are "resuming" from
                git_commit *resumingFromCommit = NULL;
                std::string resumingFromOidStdString = lastCommitIdWeGeneratedStaticGitwebFor_OrEmptyStringIfFirstRun.toStdString();
                git_oid resumingFromOid;
                if(git_oid_fromstrn(&resumingFromOid, resumingFromOidStdString.c_str(), resumingFromOidStdString.length()) != 0)
                {
                    emitLastGitError();
                    emit e("failed to convert last/resuming commit id to git_oid: " + lastCommitIdWeGeneratedStaticGitwebFor_OrEmptyStringIfFirstRun);
                    return;
                }
                if(git_commit_lookup(&resumingFromCommit, m_InputSourceRepo, &resumingFromOid) != 0)
                {
                    emitLastGitError();
                    emit e("failed to lookup the commit we are resuming from: " + lastCommitIdWeGeneratedStaticGitwebFor_OrEmptyStringIfFirstRun);
                    return;
                }
                //GitCommitFreeOnDestruct gitResumingCommitFreeOnDestruct(resumingFromCommit);
                //Q_UNUSED(gitResumingCommitFreeOnDestruct)
                parentGitCommit.reset(resumingFromCommit);
                git_tree *resumingFromTree = NULL;
                if(git_commit_tree(&resumingFromTree, resumingFromCommit) != 0)
                {
                    emitLastGitError();
                    emit e("failed to load the tree of the commit we are resuming from: " + lastCommitIdWeGeneratedStaticGitwebFor_OrEmptyStringIfFirstRun);
                    return;
                }
                gitParentTreeFreeOnDestruct.reset(resumingFromTree);
            }
            //delta diff to see which entries are new vs modified [vs renamed vs deleted]
            git_diff *gitDiff;
            if(git_diff_tree_to_tree(&gitDiff, m_InputSourceRepo, gitParentTreeFreeOnDestruct.data(), gitCurrentTreeFreeOnDestruct.data(), NULL) != 0)
            {
                emitLastGitError();
                emit e("failed to git diff tree: " + gitOidToQString(&parentOid) + " to " + oidQString);
                return;
            }
            size_t numDeltas = git_diff_num_deltas(gitDiff);
            size_t i = 0;

            while(i < numDeltas)
            {
                //these paths don't have leading slashes :)
                const git_diff_delta *diffDelta = git_diff_get_delta(gitDiff, i);
                git_delta_t diffDeltaType = diffDelta->status;
                switch(diffDeltaType)
                {
                case GIT_DELTA_ADDED:
                    //printf("new file: %s\n", diffDelta->new_file.path);
                    addedEntries.append(QString(diffDelta->new_file.path));
                    break;
                case GIT_DELTA_MODIFIED:
                    //printf("modified file: %s\n", diffDelta->new_file.path);
                    modifiedEntries.append(QString(diffDelta->new_file.path));
                    break;
                case GIT_DELTA_DELETED:
                    //deletedEntries.append() -- at first I was just being lazy/clever by doing the delete here/now instead of queing it in a list, but it seems necessary to delete all deleted entries before processing the added list... since they appear to be in arbitrary order and so a rename might(?) show it's add before it's delete, in which case i'd have no file at the end :(. this way is good though. wait what disregard that i suck cocks (renames have *gasp* new names)
                    {
                        QString filePath(diffDelta->new_file.path);
                        if(!QFile::remove(m_DestinationOutputStaticGitWebRepo + QString(diffDelta->old_file.path) + ".html"))
                        {
                            emit e("failed to delete file: " + filePath + ".html");
                            return;
                        }
                        indexHtmlFifo.removeModificationEntriesByFilePath(filePath); //remove the now-pointing-to-404 deletes in indexHtmlFifo
                        qint64 oldTimestamp = 0; // = getTimestampOfFileInPreviousCommit(filePath); //TODOreq: we're definitely not handling the root commit, but we do need to handle last modified timestamps file not existing. the trickiest case, though, is when resuming

                        const git_tree_entry *lastModifiedTimestampsEntryInParentCommitMaybe = git_tree_entry_byname(gitParentTreeFreeOnDestruct.data(), LAST_MODIFIED_TIMESTAMPS_FILEPATH);
                        if(lastModifiedTimestampsEntryInParentCommitMaybe)
                        {
                            //it existed in that tree, so pull it out (TODOoptimization: cache dat shiz) into a QTemporaryFile, and fucking finally, read the timestamp of this deleted path out of it
                            //QScopedPointer<SimplifiedLastModifiedTimestamp> parentLastModifiedTimestampsFile(new SimplifiedLastModifiedTimestamp(temporaryFile.filePath()));
                            git_otype objectType = git_tree_entry_type(lastModifiedTimestampsEntryInParentCommitMaybe);
                            if(objectType != GIT_OBJ_BLOB)
                            {
                                emit e("last modified timestamp file in parent commit was not a blob wtf");
                                return;
                            }
                            git_object *gitObject = NULL;
                            if(git_tree_entry_to_object(&gitObject, m_InputSourceRepo, lastModifiedTimestampsEntryInParentCommitMaybe) != 0)
                            {
                                emitLastGitError();
                                emit e("failed to get object from tree entry");
                                return;
                            }
                            GitObjectFreeOnDestruct gitObjectFreeOnDestruct(gitObject);
                            Q_UNUSED(gitObjectFreeOnDestruct)
                            git_blob *gitBlob = (git_blob*)gitObject;
                            const void *gitBlobRawContent = git_blob_rawcontent(gitBlob);
                            //QTemporaryFile parentLastModifiedTimestampFile
                            //shit's already in memory and my SimplifiedLastModifiedTimestamp doesn't have a file reading helper like I thought so eh no point in writing it
                            QByteArray gitBlobBA = QByteArray::fromRawData((const char*)gitBlobRawContent, git_blob_rawsize(gitBlob));
                            QTextStream gitBlobTextStream(gitBlobBA, QIODevice::ReadOnly);
                            while(!gitBlobTextStream.atEnd())
                            {
                                const QString &currentLine = gitBlobTextStream.readLine();
                                if(currentLine.trimmed().isEmpty())
                                    continue;
                                SimplifiedLastModifiedTimestamp currentTimestampEntry(currentLine);
                                if(!currentTimestampEntry.isValid())
                                {
                                    emit e("came upon an invalid timestamp entry in last modified timestamps: " + currentLine);
                                    return;
                                }
                                if((!currentTimestampEntry.isDirectory()) && currentTimestampEntry.filePath() == filePath)
                                {
                                    oldTimestamp = currentTimestampEntry.lastModifiedTimestamp().toMSecsSinceEpoch() / 1000;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            //it doesn't exist, so use the parent's commit timestamp instead
                            if(parentGitCommit.isNull())
                            {
                                //this should never happen
                                emit e("error: parent git commit is null and we need it's timestamp to find out where a deleted file is in the archive htmls");
                                return;
                            }
                            oldTimestamp = git_commit_time(parentGitCommit.data());
                        }

                        if(oldTimestamp == 0)
                        {
                            emit e("failed to get the old timestamp of a deleted entry: " + filePath);
                            return;
                        }
                        QDateTime oldTimestampDateTime = QDateTime::fromMSecsSinceEpoch(oldTimestamp * 1000);

                        ArchiveHtmlDeletedFileEntryType archiveHtmlDeletedFileEntryType(oldTimestampDateTime.date().year(), oldTimestampDateTime.date().month(), filePath.toHtmlEscaped());
                        archiveHtmls.addDeletedFileEntry(archiveHtmlDeletedFileEntryType);
                    }
                    break;
                default:
                    //TODOmb: GIT_DELTA_TYPECHANGE shows up as a delete/add (at same file path, I presume). would be better to just leave it as is (and not bump it to the top of the indexHtml). I don't care about file permissions in this html stuff (might later, but doubt it)
                    //TODOmb: detect renames and present them as modifications (link to the diff). just more work (for me.... AND the cpu :-P)
                    //printf("other blah file: %s\n", diffDelta->new_file.path);
                    break;
                }
                ++i;
            }

            if(!modifiedEntries.isEmpty())
            {
                //0) MAKE DIFF COLORIZED HTML FILE if any modifified files in the diff (we don't generate a colorized diff html file when every file in the diff was simply an "add/create"
                QStringList gitShowArgs = QStringList() << "show" << "--color-words" << oidQString;
                m_GitProcess = new QProcess(this);
                m_GitProcess->setWorkingDirectory(sourceInputRepoFilePath);
                if(!runGit(gitShowArgs))
                    return;
                QByteArray ansiColorizedGitShowOutput = m_GitProcess->readAll();
                m_Ansi2htmlProcess = new QProcess(this);
                //none: QStringList ansi2htmlArgs = QStringList() << ""
                if(!runAnsi2Html(ansiColorizedGitShowOutput))
                    return;
                QTextStream ansi2HtmlProcessTextStream(m_Ansi2htmlProcess);
                QString ansi2HtmlOutput = ansi2HtmlProcessTextStream.readAll();
                diffHtmlFilePath = diffFileName(m_HandlingRootCommit ? "000000000" : gitOidToQString(&parentOid).left(9), oidQString.left(9)) + ".html";
                if(!ensureAllParentDirectoriesExistMakingThemIfNeeded(diffHtmlFilePath))
                    return;
                if(!makeFileInDestinationOutputStaticGitWebRepoIfItDoesntAlreadyExist(diffHtmlFilePath, ansi2HtmlOutput))
                    return;
            }
        }

        //1a) [OVER-]WRITE TREE
        //1b) Delete deletes+rename-sources (or just delete entire tree before writing?). TODOmb: renames COULD use "moved temporarily" redirect to new filename (moved permanently would mean I could/should never use that filename again. I wish there was a moved-probably-permanently ;-P)
        //when handlingRootCommit == true, we can't use a delta diff to figure out the files added (or at last, idk how :-P). so fuck it yea ima just do a recursive directory walk and mark all the filenames myself via QDirIterator!! actually fuck it man it's probably just as difficult to iterate the tree using libgit2 and mark the filenames. It's worth noting that ALL entries in the root tree are going to be additions, so that simplifies things. In any case, I'll be detecting modifications/additions (deletions/renames too!) for handlingRootCommit == false using delta diffing (which I've already prototyped)
        m_TreeWalkError = false;
        m_TreeEntryPathsEitherAddedOrModified.clear();
        m_AddedEntriesLongerLived = addedEntries;
        m_ModifiedEntriesLongerLived = modifiedEntries;
        if(((git_tree_walk(tree, GIT_TREEWALK_PRE, staticTreeWalkingCallback, this) != 0)) || m_TreeWalkError)
        {
            emitLastGitError();
            emit e("error whiile tree walking: " + oidQString);
            return;
        }
        //if(!touchAllTreeEntriesEitherAddedOrModified())
        //    return;
        std::sort(m_TreeEntryPathsEitherAddedOrModified.begin(), m_TreeEntryPathsEitherAddedOrModified.end()); //TODOmb: sort by timestamp instead? (but if there is none, use alphabetical like this)

        if(m_TreeEntryPathsEitherAddedOrModified.contains(LAST_MODIFIED_TIMESTAMPS_FILEPATH))
        {
            LastModifiedTimestampsSorter timestampsSorter;
            int sortedTimestampsCount;
            QScopedPointer<SortedMapOfListsOfPathsPointerType, SortedMapOfListsOfPathsPointerTypeDeleter> sortedMap(timestampsSorter.sortLastModifiedTimestamps(m_DestinationOutputStaticGitWebRepo + LAST_MODIFIED_TIMESTAMPS_FILEPATH, &sortedTimestampsCount));

//            for(SortedMapOfListsOfPathsPointerType::const_iterator sortedMapIterator = sortedMap->constBegin(); sortedMapIterator != sortedMap->end(); ++sortedMapIterator)
//            {
//                QList<std::string> listOfPathsAtGivenTimestamp = (*sortedMapIterator)->at()
//            }
            QMapIterator<long long, QList<std::string>*> sortedMapIterator(*sortedMap.data());
            while(sortedMapIterator.hasNext())
            {
                sortedMapIterator.next();
                long long timestamp = sortedMapIterator.key();
                Q_FOREACH(const std::string &currentPathWithThisTimestamp, (*sortedMapIterator.value()))
                {
                    QPair<QString, QString> currentPath_AndHtmlIzedPath;
                    currentPath_AndHtmlIzedPath.first = QString::fromStdString(currentPathWithThisTimestamp);
                    QString currentPathHtmlEscaped = currentPath_AndHtmlIzedPath.first.toHtmlEscaped();
                    if(m_HandlingRootCommit)
                    {
                        if(!touchFileInDestinationDir(currentPath_AndHtmlIzedPath.first, timestamp))
                            return;
                        currentPath_AndHtmlIzedPath.second = createdFileHtml(currentPathHtmlEscaped, timestamp);
                        indexHtmlFifo.myEnqueue(currentPath_AndHtmlIzedPath);
                        archiveHtmls.addCreatedFileEntry(ArchiveHtmlCreatedFileEntryType(timestamp, currentPathHtmlEscaped));
                    }
                    else
                    {
                        if(addedEntries.removeOne(currentPath_AndHtmlIzedPath.first))
                        {
                            if(!touchFileInDestinationDir(currentPath_AndHtmlIzedPath.first, timestamp))
                                return;
                            currentPath_AndHtmlIzedPath.second = createdFileHtml(currentPathHtmlEscaped, timestamp);
                            indexHtmlFifo.myEnqueue(currentPath_AndHtmlIzedPath);
                            archiveHtmls.addCreatedFileEntry(ArchiveHtmlCreatedFileEntryType(timestamp, currentPathHtmlEscaped));
                        }
                        else if(modifiedEntries.removeOne(currentPath_AndHtmlIzedPath.first)) //TO DOnereq: modifications of the same file "in a given 100 most recent delta diffs" should first remove their prior entry in the indexHtmlFifo (whether an "add" or a "modify"). Doing so (like described!) will keep the count at exactly 100 (nothing needs to be dropped from indexHtmlFifo when enqueue is called), and also avoids duplicates which are dumb. in fact maybe indexHtmlFifo should be in charge of dedupe'ing like that :-P. I'm now changing it to accept a QPair for this reason
                        {
                            if(!touchFileInDestinationDir(currentPath_AndHtmlIzedPath.first, timestamp))
                                return;
                            currentPath_AndHtmlIzedPath.second = modifiedFileHtml(diffHtmlFilePath, currentPathHtmlEscaped, timestamp);
                            indexHtmlFifo.myEnqueue(currentPath_AndHtmlIzedPath);


                            qint64 oldTimestamp = 0; // = getTimestampOfFileInPreviousCommit(filePath); //TODOreq: we're definitely not handling the root commit, but we do need to handle last modified timestamps file not existing. the trickiest case, though, is when resuming

                            const git_tree_entry *lastModifiedTimestampsEntryInParentCommitMaybe = git_tree_entry_byname(gitParentTreeFreeOnDestruct.data(), LAST_MODIFIED_TIMESTAMPS_FILEPATH);
                            if(lastModifiedTimestampsEntryInParentCommitMaybe)
                            {
                                //it existed in that tree, so pull it out (TODOoptimization: cache dat shiz) into a QTemporaryFile, and fucking finally, read the old timestamp of this modified path out of it
                                git_otype objectType = git_tree_entry_type(lastModifiedTimestampsEntryInParentCommitMaybe);
                                if(objectType != GIT_OBJ_BLOB)
                                {
                                    emit e("last modified timestamp file in parent commit was not a blob wtf");
                                    return;
                                }
                                git_object *gitObject = NULL;
                                if(git_tree_entry_to_object(&gitObject, m_InputSourceRepo, lastModifiedTimestampsEntryInParentCommitMaybe) != 0)
                                {
                                    emitLastGitError();
                                    emit e("failed to get object from tree entry");
                                    return;
                                }
                                GitObjectFreeOnDestruct gitObjectFreeOnDestruct(gitObject);
                                Q_UNUSED(gitObjectFreeOnDestruct)
                                git_blob *gitBlob = (git_blob*)gitObject;
                                const void *gitBlobRawContent = git_blob_rawcontent(gitBlob);
                                QByteArray gitBlobBA = QByteArray::fromRawData((const char*)gitBlobRawContent, git_blob_rawsize(gitBlob));
                                QTextStream gitBlobTextStream(gitBlobBA, QIODevice::ReadOnly);
                                while(!gitBlobTextStream.atEnd())
                                {
                                    const QString &currentLine = gitBlobTextStream.readLine();
                                    if(currentLine.trimmed().isEmpty())
                                        continue;
                                    SimplifiedLastModifiedTimestamp currentTimestampEntry(currentLine);
                                    if(!currentTimestampEntry.isValid())
                                    {
                                        emit e("came upon an invalid timestamp entry in last modified timestamps: " + currentLine);
                                        return;
                                    }
                                    if((!currentTimestampEntry.isDirectory()) && currentTimestampEntry.filePath() == currentPath_AndHtmlIzedPath.first)
                                    {
                                        oldTimestamp = currentTimestampEntry.lastModifiedTimestamp().toMSecsSinceEpoch() / 1000;
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                //it didn't exist, so use the parent's commit timestamp instead
                                if(parentGitCommit.isNull())
                                {
                                    //this should never happen
                                    emit e("error: parent git commit is null and we need it's timestamp to find out where a modified file is in the archive htmls");
                                    return;
                                }
                                oldTimestamp = git_commit_time(parentGitCommit.data());
                            }

                            if(oldTimestamp == 0)
                            {
                                if(!parentGitCommit.isNull())
                                    oldTimestamp = git_commit_time(parentGitCommit.data());
                                if(oldTimestamp == 0)
                                {
                                    emit e("failed to get the old timestamp of a modified entry: " + currentPath_AndHtmlIzedPath.first);
                                    return;
                                }
                            }
                            ArchiveHtmlModifiedFileEntryType archiveHtmlModifiedFileEntryType(oldTimestamp, timestamp, currentPathHtmlEscaped);

                            archiveHtmls.addModifiedFileEntry(archiveHtmlModifiedFileEntryType);
                        }
                    }
                }

                //optimization:
                if((!m_HandlingRootCommit) && modifiedEntries.isEmpty() && addedEntries.isEmpty())
                    break;
            }
        }
        else
        {
            //TODOreq: give them all the [same] commit timestamp, and we still need to add them to the indexHtmlFifo in reverse alphabetical order (since we iterate indexHtmlFifo backwards when writing the index.html)
            git_time_t commitTime = git_commit_time(commit);
            QListIterator<QString> currentTreeEntryPathsIterator(m_TreeEntryPathsEitherAddedOrModified);
            currentTreeEntryPathsIterator.toBack();
            while(currentTreeEntryPathsIterator.hasPrevious())
            {
                QPair<QString, QString> indexHtmlEntry;
                indexHtmlEntry.first = currentTreeEntryPathsIterator.previous();
                QString currentPathHtmlEscaped = indexHtmlEntry.first.toHtmlEscaped();
                if(m_HandlingRootCommit)
                {
                    if(!touchFileInDestinationDir(indexHtmlEntry.first, commitTime))
                        return;
                    indexHtmlEntry.second = createdFileHtml(currentPathHtmlEscaped, commitTime);
                    indexHtmlFifo.myEnqueue(indexHtmlEntry);
                    archiveHtmls.addCreatedFileEntry(ArchiveHtmlCreatedFileEntryType(commitTime, currentPathHtmlEscaped));
                }
                else
                {
                    if(addedEntries.removeOne(indexHtmlEntry.first))
                    {
                        if(!touchFileInDestinationDir(indexHtmlEntry.first, commitTime))
                            return;
                        indexHtmlEntry.second = createdFileHtml(currentPathHtmlEscaped, commitTime);
                        indexHtmlFifo.myEnqueue(indexHtmlEntry);
                        archiveHtmls.addCreatedFileEntry(ArchiveHtmlCreatedFileEntryType(commitTime, currentPathHtmlEscaped));
                    }
                    else if(modifiedEntries.removeOne(indexHtmlEntry.first))
                    {
                        if(!touchFileInDestinationDir(indexHtmlEntry.first, commitTime))
                            return;
                        indexHtmlEntry.second = modifiedFileHtml(diffHtmlFilePath, currentPathHtmlEscaped, commitTime);
                        indexHtmlFifo.myEnqueue(indexHtmlEntry);


                        qint64 oldTimestamp = 0; // = getTimestampOfFileInPreviousCommit(filePath); //TODOreq: we're definitely not handling the root commit, but we do need to handle last modified timestamps file not existing. the trickiest case, though, is when resuming

                        const git_tree_entry *lastModifiedTimestampsEntryInParentCommitMaybe = git_tree_entry_byname(gitParentTreeFreeOnDestruct.data(), LAST_MODIFIED_TIMESTAMPS_FILEPATH);
                        if(lastModifiedTimestampsEntryInParentCommitMaybe)
                        {
                            //it existed in that tree, so pull it out (TODOoptimization: cache dat shiz) into a QTemporaryFile, and fucking finally, read the old timestamp of this modified path out of it
                            git_otype objectType = git_tree_entry_type(lastModifiedTimestampsEntryInParentCommitMaybe);
                            if(objectType != GIT_OBJ_BLOB)
                            {
                                emit e("last modified timestamp file in parent commit was not a blob wtf");
                                return;
                            }
                            git_object *gitObject = NULL;
                            if(git_tree_entry_to_object(&gitObject, m_InputSourceRepo, lastModifiedTimestampsEntryInParentCommitMaybe) != 0)
                            {
                                emitLastGitError();
                                emit e("failed to get object from tree entry");
                                return;
                            }
                            GitObjectFreeOnDestruct gitObjectFreeOnDestruct(gitObject);
                            Q_UNUSED(gitObjectFreeOnDestruct)
                            git_blob *gitBlob = (git_blob*)gitObject;
                            const void *gitBlobRawContent = git_blob_rawcontent(gitBlob);
                            QByteArray gitBlobBA = QByteArray::fromRawData((const char*)gitBlobRawContent, git_blob_rawsize(gitBlob));
                            QTextStream gitBlobTextStream(gitBlobBA, QIODevice::ReadOnly);
                            while(!gitBlobTextStream.atEnd())
                            {
                                const QString &currentLine = gitBlobTextStream.readLine();
                                if(currentLine.trimmed().isEmpty())
                                    continue;
                                SimplifiedLastModifiedTimestamp currentTimestampEntry(currentLine);
                                if(!currentTimestampEntry.isValid())
                                {
                                    emit e("came upon an invalid timestamp entry in last modified timestamps: " + currentLine);
                                    return;
                                }
                                if((!currentTimestampEntry.isDirectory()) && currentTimestampEntry.filePath() == currentPathHtmlEscaped)
                                {
                                    oldTimestamp = currentTimestampEntry.lastModifiedTimestamp().toMSecsSinceEpoch() / 1000;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            //it didn't exist, so use the parent's commit timestamp instead
                            if(parentGitCommit.isNull())
                            {
                                //this should never happen
                                emit e("error: parent git commit is null and we need it's timestamp to find out where a modified file is in the archive htmls");
                                return;
                            }
                            oldTimestamp = git_commit_time(parentGitCommit.data());
                        }

                        if(oldTimestamp == 0)
                        {
                            if(!parentGitCommit.isNull())
                                oldTimestamp = git_commit_time(parentGitCommit.data());
                            if(oldTimestamp == 0)
                            {
                                emit e("failed to get the old timestamp of a modified entry: " + currentPathHtmlEscaped);
                                return;
                            }
                        }
                        ArchiveHtmlModifiedFileEntryType archiveHtmlModifiedFileEntryType(oldTimestamp, commitTime, currentPathHtmlEscaped);

                        archiveHtmls.addModifiedFileEntry(archiveHtmlModifiedFileEntryType);
                    }

                    //optimization:
                    if(modifiedEntries.isEmpty() && addedEntries.isEmpty())
                        break;
                }
            }
        }

        //4) SITEMAP

        //3) GIT ADD/COMMIT

        parentOid = oid;
        m_HandlingRootCommit = false;
        parentGitCommit.reset(gitCommitFreeOnDestruct.take());
        emit e("Commits processed so far: " + QString::number(++numCommitsProcessed));
    }

    //2) Write INDEX.HTML (aka "last 100 entries)
    QString indexHtml = "<html><head><title>d3fault.net</title>";
    indexHtml.append(cssStyleTag());
    indexHtml.append("</head>\n");
    indexHtml.append(bodyHeader());
    QListIterator<QPair<QString, QString> > indexHtmlFifoIterator(indexHtmlFifo);
    indexHtmlFifoIterator.toBack(); //iterate in reverse, because our tail end has the "newest" entries, which we want to show at the top of our index.html file :-P
    indexHtml.append("<center><table>");
    bool even = true;
    QList<QPair<QString, QString> > ehUpdateTheseWithoutMovingPositionAfterYouGetDoneIteratingPlx;
    while(indexHtmlFifoIterator.hasPrevious())
    {
        indexHtml.append("<tr class='" + QString(even ? "d0" : "d1") + "'><td>");
        QPair<QString, QString> indexHtmlEntry = indexHtmlFifoIterator.previous();

        QString tehHtmlz = indexHtmlEntry.second;
        //if((!tehHtmlz.contains(A_STRING_IN_MODIFIED_FILE_HTML_KEK_HACK)) && (!))
        if(tehHtmlz.isEmpty())
        {
            //OLD: that string (hack) tells us that this entry is NOT a modification. it also must be empty, otherwise we populated it in a last session [and don't even have the plaintext (non-htmlized) copy of it handy anymore]

            QString pathHtmlEscapedActualFileName = indexHtmlEntry.first.toHtmlEscaped();
            QString pathHtmlEscaped = pathHtmlEscapedActualFileName + ".html";
            QFileInfo theHtmlFileInfo = m_DestinationOutputStaticGitWebRepo + indexHtmlEntry.first + ".html";
            tehHtmlz.append(theHtmlFileInfo.lastModified().toString(QUICK_DIRTY_STATIC_GIT_WEB_DATETIME_FORMAT_STRING) + " -- d3fault created the file: <a href='" + pathHtmlEscaped + "'>" + pathHtmlEscapedActualFileName + "</a><br /><pre><blockquote cite='" + pathHtmlEscaped + "'>");

            QFile file(appendSlashIfNeeded(m_TempDirOfUnHtmlizedFileContentsOfFilesAddedOrModifiedDuringThisAppSession->path()) + indexHtmlEntry.first);
            if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                emit e("error opening file for reading: " + file.fileName());
                return;
            }
            QTextStream textStream(&file);
            tehHtmlz.append(textStream.readAll().toHtmlEscaped());

            tehHtmlz.append("</blockquote></pre>");
            QPair<QString, QString> newPathAndHtml;
            newPathAndHtml.first = indexHtmlEntry.first;
            newPathAndHtml.second = tehHtmlz;
            ehUpdateTheseWithoutMovingPositionAfterYouGetDoneIteratingPlx.append(newPathAndHtml); //store tehHtmlz in the .conf shiz, because we won't have the file in the temp dir on next app session!
        }
        indexHtml.append(tehHtmlz);
        indexHtml.append("</td></tr>\n");
        even = !even;
    }
    QListIterator<QPair<QString, QString> > delayedUpdatesIterator(ehUpdateTheseWithoutMovingPositionAfterYouGetDoneIteratingPlx);
    while(delayedUpdatesIterator.hasNext())
    {
        QPair<QString, QString> aDelayedUpdate = delayedUpdatesIterator.next();
        indexHtmlFifo.updateSecondWithoutMovingPositionInQueue(aDelayedUpdate);
    }
    indexHtml.append("</table></center>");
#if 0
    int numEntries = 0;
    Q_FOREACH(const QString &treeEntryPath, m_CurrentTreeEntryPaths)
    {
        QString treeEntryPathHtmlEscaped = treeEntryPath.toHtmlEscaped();
        indexHtml.append("d3fault created: "); //had: "added the file:" instead of "created:"
        indexHtml.append("<a href='" + treeEntryPathHtmlEscaped + "'>" + treeEntryPathHtmlEscaped + "</a>"); //TODOreq: show the file contents (which were available to us during the tree walk where we already wrote them to file!), perhaps wth a high upper limit such as 1024 characters, using <blockquote> (cited). Use elipsis if truncated
        indexHtml.append("<br />\n");
        ++numEntries;
        if(numEntries >= MAX_INDEXHTML_ENTRIES)
            break;
    }
#endif
    //indexHtml.append("<br /><a href='archive.html'>Archive</a></body></html>\n");
    indexHtml.append(bodyFooter());
    if(!makeFileInDestinationOutputStaticGitWebRepoOverwritingIfNecessary("index.html", indexHtml))
        return;

    //3) ARCHIVE
    archiveHtmls.applyMutations(); //we didn't make the archives "on teh fly" because they'd have to be deleted/overwritten too much
    if(!rewriteStaleArchiveHtmls(&archiveHtmls))
        return;
    //resetAllToNotStaleForNextRun();


    settings.setValue(LAST_COMMIT_ID_WE_GENERATED_STATIC_GITWEB_FOR_SETTINGS_KEY, gitOidToQString(&parentOid)); //TODOmb: various error cases should still set this (so do it in a scope destruct)? like if some of the commitIDs succeed before one of the child ones fails, we don't want to redo the succeeded ones

    //4) GIT PUSH? maybe that's a second line of the post-update script (the first line calling this very app) instead of being in here, since this app should only do "one thing really well" and that one thing is generating the STATIC gitweb for a git repo (which, incidentally, is another git repo :-P)

    emit e("success");
    resultEmitter.data()->setSuccess(true);
    //OT: if I wanted to wait for an async op, I could take() the scoped pointer and put it into a member one. Then, immediately after that async op finished, I take() it out of the member one and put it in another local scoped pointer (local to that async callback (slot in qt land)). It might be a good idea to check that the member scoped pointer isn't null also, but it being null would indicate a design error (which DO happen!)
}
QuickDirtyStaticGitWebResultEmitter::QuickDirtyStaticGitWebResultEmitter(QObject *parent)
    : QObject(parent)
    , m_Success(false)
{ }
QuickDirtyStaticGitWebResultEmitter::~QuickDirtyStaticGitWebResultEmitter()
{
    emit finishedGeneratingStaticGitWeb(m_Success);
}
void QuickDirtyStaticGitWebResultEmitter::setSuccess(bool success)
{
    m_Success = success;
}
void ArchiveHtmls::addCreatedFileEntry(const ArchiveHtmlCreatedFileEntryType &createdFileEntry)
{
    if(createdFileEntry.Path == LAST_MODIFIED_TIMESTAMPS_FILEPATH)
        return;
    m_CreatedFileEntries.append(createdFileEntry);
}
void ArchiveHtmls::addModifiedFileEntry(const ArchiveHtmlModifiedFileEntryType &modifiedFileEntry)
{
    if(modifiedFileEntry.Path == LAST_MODIFIED_TIMESTAMPS_FILEPATH)
        return;
    //TODOreq: these are handled KINDA like deletes, at least in the context of the archive htmls. The old entry in the archive must be found and deleted! and then after that they are just like a created file entry kek. I can/prolly-will share the functionality via functions (whoa- O_O)
    m_ModifiedFileEntries.append(modifiedFileEntry);
}
void ArchiveHtmls::addDeletedFileEntry(const ArchiveHtmlDeletedFileEntryType &deletedFileEntry)
{
    m_DeletedFileEntries.append(deletedFileEntry);
}
ArchiveYearHtml* ArchiveHtmls::getOrCreateYearArchiveHtmlByYear(int year)
{
    YearsArchiveHtmlsType::const_iterator it = m_YearsArchiveHtmls.find(year);
    if(it == m_YearsArchiveHtmls.end())
    {
        m_Stale = true;
        it = m_YearsArchiveHtmls.insert(year, QSharedPointer<ArchiveYearHtml>(new ArchiveYearHtml(this, year)));
    }
    return it.value().data();
}
void ArchiveHtmls::applyMutations() //I can't remember the purpose of this method. Why do I delay the application of mutations (essentially the marking of stale)? I'm thinking now that it's unecessary work, but fuck uncoding it when I'm unsure there isn't a reason
{
    Q_FOREACH(const ArchiveHtmlDeletedFileEntryType &deletedFileEntry, m_DeletedFileEntries)
    {
        //m_YearsArchiveHtmls.value(deletedFileEntry.year())->monthArchiveHtmls().value(deletedFileEntry.month())->removeEntryByPath(deletedFileEntry.path());
        getOrCreateYearArchiveHtmlByYear(deletedFileEntry.Year)->getOrCreateMonthArchiveHtmlByMonth(deletedFileEntry.Month)->removeEntryByPath(deletedFileEntry.Path);
    }
    m_DeletedFileEntries.clear();
    Q_FOREACH(const ArchiveHtmlModifiedFileEntryType &modifiedFileEntry, m_ModifiedFileEntries)
    {
        //m_YearsArchiveHtmls.value(modifiedFileEntry.yearBeforeThisMostRecentModification())->monthArchiveHtmls().value(modifiedFileEntry.monthBeforeThisMostRecentModification())->removeEntryByPath(modifiedFileEntry.path());
        QDateTime oldTimestamp = QDateTime::fromMSecsSinceEpoch(modifiedFileEntry.OldTimestamp * 1000);
        getOrCreateYearArchiveHtmlByYear(oldTimestamp.date().year())->getOrCreateMonthArchiveHtmlByMonth(oldTimestamp.date().month())->removeEntryByPath(modifiedFileEntry.Path);

        //m_YearsArchiveHtmls.value(modifiedFileEntry.yearOfMostRecentModification())->monthArchiveHtmls().value(modifiedFileEntry.monthOfMostRecentModification())->addEntry(modifiedFileEntry.timestampOfMostRecentModification(), modifiedFileEntry.path());
        QDateTime newTimestamp = QDateTime::fromMSecsSinceEpoch(modifiedFileEntry.NewTimestamp * 1000);
        getOrCreateYearArchiveHtmlByYear(newTimestamp.date().year())->getOrCreateMonthArchiveHtmlByMonth(newTimestamp.date().month())->addEntry(modifiedFileEntry.NewTimestamp, modifiedFileEntry.Path);
    }
    m_ModifiedFileEntries.clear();
    Q_FOREACH(const ArchiveHtmlCreatedFileEntryType &createdFileEntry, m_CreatedFileEntries)
    {
        //m_YearsArchiveHtmls.value(createdFileEntry.yearOfMostRecentModification())->monthArchiveHtmls().value(createdFileEntry.monthOfMostRecentModification())->addEntry(createdFileEntry.timestampOfMostRecentModification(), createdFileEntry.path());
        QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(createdFileEntry.Timestamp * 1000);
        getOrCreateYearArchiveHtmlByYear(dateTime.date().year())->getOrCreateMonthArchiveHtmlByMonth(dateTime.date().month())->addEntry(createdFileEntry.Timestamp, createdFileEntry.Path);
    }
    m_CreatedFileEntries.clear();
}
void ArchiveHtmls::removeYear(ArchiveYearHtml *yearToRemove)
{
    m_YearsArchiveHtmls.remove(yearToRemove->year());
    m_Stale = true;
}
QSet<ArchiveYearHtml *> ArchiveHtmls::staleYears() const
{
    return m_StaleYears;
}
QSet<ArchiveMonthHtml *> ArchiveHtmls::staleMonths() const
{
    return m_StaleMonths;
}
void ArchiveHtmls::addStaleYear(ArchiveYearHtml *staleYear)
{
    m_StaleYears.insert(staleYear);
}
void ArchiveHtmls::addStaleMonth(ArchiveMonthHtml *staleMonth)
{
    m_StaleMonths.insert(staleMonth);
}
ArchiveMonthHtml::ArchiveMonthHtml(ArchiveYearHtml *parentArchiveYearHtml, int month)
//: m_Stale(false)
: m_ParentArchiveYear(parentArchiveYearHtml)
, m_Month(month)
{
    m_ParentArchiveYear->parentArchiveHtmls()->addStaleMonth(this);
}
void ArchiveMonthHtml::addEntry(qint64 timestamp, const QString &path)
{
    m_Entries.insert(timestamp, path);
    //m_Stale = true;
    m_ParentArchiveYear->parentArchiveHtmls()->addStaleMonth(this);
}
void ArchiveMonthHtml::removeEntryByPath(const QString &path)
{
    //QMutableMapIterator<ARCHIVE_MONTH_HTML_ENTRY_MAP_TYPES> entryIterator(m_Entries);
    m_Entries.remove(m_Entries.key(path, 0));
    //m_Stale = true;
    m_ParentArchiveYear->parentArchiveHtmls()->addStaleMonth(this);
}
ArchiveYearHtml *ArchiveMonthHtml::parentArchiveYear() const
{
    return m_ParentArchiveYear;
}
int ArchiveMonthHtml::month() const
{
    return m_Month;
}
QMultiMap<qint64, QString> ArchiveMonthHtml::entries() const
{
    return m_Entries;
}
ArchiveYearHtml::ArchiveYearHtml(ArchiveHtmls *parentArchiveHtmls, int year)
    : m_ParentArchiveHtmls(parentArchiveHtmls)
    , m_Year(year)
{
    m_ParentArchiveHtmls->addStaleYear(this);
}
ArchiveMonthHtml *ArchiveYearHtml::getOrCreateMonthArchiveHtmlByMonth(int month)
{
    MonthsArchiveHtmlsType::const_iterator it = m_MonthsArchiveHtmls.find(month);
    if(it == m_MonthsArchiveHtmls.end())
    {
        m_ParentArchiveHtmls->addStaleYear(this);
        it = m_MonthsArchiveHtmls.insert(month, QSharedPointer<ArchiveMonthHtml>(new ArchiveMonthHtml(this, month)));
    }
    return it.value().data();
}
ArchiveHtmls *ArchiveYearHtml::parentArchiveHtmls() const
{
    return m_ParentArchiveHtmls;
}
int ArchiveYearHtml::year() const
{
    return m_Year;
}
void ArchiveYearHtml::removeMonth(ArchiveMonthHtml *month)
{
    m_MonthsArchiveHtmls.remove(month->month());
    m_ParentArchiveHtmls->addStaleYear(this);
}
