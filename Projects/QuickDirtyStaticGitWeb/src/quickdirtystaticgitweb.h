#ifndef QUICKDIRTYSTATICGITWEB_H
#define QUICKDIRTYSTATICGITWEB_H

#include <QObject>
#include <QScopedPointer>
#include <QStringList>
#include <QQueue>
#include <QSettings>
#include <QMap>
#include <QSet>
#include <QSharedPointer>
#include <QTemporaryDir>

extern "C"
{
    #include <git2.h>
}

#include "lastmodifiedtimestampssorter.h"

#define MAX_INDEXHTML_ENTRIES 100
#define LAST_MODIFIED_TIMESTAMPS_FILEPATH ".lastModifiedTimestamps"
#define MONTHS_ARCHIVE_HTMLS_MAP_TYPES int /*month*/, QSharedPointer<ArchiveMonthHtml>
#define A_STRING_IN_MODIFIED_FILE_HTML_KEK_HACK "modified</a> the file: <a href" //our blockquoted text MIGHT have this text in it.... BUT we htmlencode that before showing it, so it's impossible for it to contain this string with the < and > etc... kek

class QuickDirtyStaticGitWebResultEmitter;

typedef QScopedPointer<QuickDirtyStaticGitWebResultEmitter> ScopedQuickDirtyStaticGitWebResultEmitterTypedef;

class QProcess;
class ArchiveHtmls;

class QuickDirtyStaticGitWeb : public QObject
{
    Q_OBJECT
public:
    explicit QuickDirtyStaticGitWeb(QObject *parent = 0);
    int treeWalkingCallback(const char *root, const git_tree_entry *entry);
private:
    bool m_HandlingRootCommit;
    git_repository *m_InputSourceRepo;
    QString m_DestinationOutputStaticGitWebRepo;
    QProcess *m_GitProcess;
    QProcess *m_Ansi2htmlProcess;
    bool runAnsi2Html(const QByteArray &ansi2htmlInput);
    bool runGit(QStringList gitArgs);
    bool m_TreeWalkError;
    QStringList m_TreeEntryPathsEitherAddedOrModified;
    QStringList m_AddedEntriesLongerLived;
    QStringList m_ModifiedEntriesLongerLived;
    QScopedPointer<QTemporaryDir> m_TempDirOfUnHtmlizedFileContentsOfFilesAddedOrModifiedDuringThisAppSession;

    QString gitOidToQString(const git_oid *oid);
    void emitLastGitError();
    QPair<bool, QString> getHeadCommitId();
    static inline QString diffFileName(const QString &diffFromCommitId, const QString &diffToCommitId) { return "diff/" + diffFromCommitId + ".." + diffToCommitId; }
    QString createdFileHtml(const QString &htmlEncodedFilePath, qint64 fileTimestamp/*, const QString &unencodedFilePath*/);
    QString modifiedFileHtml(const QString &diffHtmlFilePath, const QString &htmlEncodedFilePath, qint64 modificationTimestamp);
    bool ensureAllParentDirectoriesExistInTempPlaintextDirMakingThemIfNeeded(const QString &filePathOf_FILE_notDir);
    bool ensureAllParentDirectoriesExistMakingThemIfNeeded(const QString &filePathOf_FILE_notDir);
    bool makeFileOverwritingIfNecessary(const QString &absoluteFilePath, const QString &fileContents);
    bool makeFileInTempPlaintextDirOverwritingIfNecessary(const QString &relativeFilePath, const QString &fileContents);
    bool makeFileInDestinationOutputStaticGitWebRepoOverwritingIfNecessary(const QString &relativeFilePath, const QString &fileContents);
    bool makeFileInDestinationOutputStaticGitWebRepoIfItDoesntAlreadyExist(const QString &relativeFilePath, const QString &fileContents);
    bool rewriteStaleArchiveHtmls(ArchiveHtmls *archiveHtmls);
    //bool touchAllTreeEntriesEitherAddedOrModified();
    bool touchFileInDestinationDir(const QString &relativePath, qint64 timestamp);
    QMultiMap<qint64 /*timestamp*/, QString /*path*/> getAllEntriesForThisMonthNotJustMutatedInThisAppSession(int year, int month) const;
    QMap<int, QString> getAllMonthsWithEntriesForThisYearNotJustMutatedInThisAppSession(int year);
    QMap<int, QString> getAllYearsWithMonthEntriesNotJustMutatedInThisAppSession();
    QMap<int, QMap<int, bool> > getAllMonthsArchiveHtmls();
    void makeStaticAboutFileIfItNeedsToChange();
    void makeStaticCNAMEIfItDoesntExist();
    void makeStaticRobotsIfItDoesntExist();
    QString cssStyleTag();

    static QString bodyHeader();
    static QString bodyFooter();
    static inline QString appendSlashIfNeeded(const QString &inputString) { if(inputString.endsWith("/")) return inputString; return QString(inputString + "/"); }
    static inline QString removeLeadingSlashIfExisting(const QString &inputString) { if(!inputString.startsWith("/")) return inputString; QString ret(inputString); ret.remove(0, 1); return ret; }
signals:
    void e(const QString &msg);
    void finishedGeneratingStaticGitWeb(bool success);
public slots:
    void generateStaticGitWeb(const QString &sourceInputRepo, const QString &destinationOutputStaticGitWebRepo);
};

class QuickDirtyStaticGitWebResultEmitter : public QObject
{
    Q_OBJECT
public:
    explicit QuickDirtyStaticGitWebResultEmitter(QObject *parent = 0);
    ~QuickDirtyStaticGitWebResultEmitter();
    void setSuccess(bool success);
private:
    bool m_Success;
signals:
    void finishedGeneratingStaticGitWeb(bool success);
};
class GitRepositoryFreeOnDestruct
{
public:
    GitRepositoryFreeOnDestruct(git_repository *repoToFree)
        : m_RepoToFree(repoToFree)
    { }
    ~GitRepositoryFreeOnDestruct()
    {
        git_repository_free(m_RepoToFree);
    }
private:
    git_repository *m_RepoToFree;
};
class GitRevwalkFreeOnDestruct
{
public:
    GitRevwalkFreeOnDestruct(git_revwalk *revwalkToFree)
        : m_RevwalkToFree(revwalkToFree)
    { }
    ~GitRevwalkFreeOnDestruct()
    {
        git_revwalk_free(m_RevwalkToFree);
    }
private:
    git_revwalk *m_RevwalkToFree;
};
struct ScopedPointerCustomDeleter
{
    static inline void cleanup(git_tree *pointer)
    {
        git_tree_free(pointer);
    }
};
typedef QScopedPointer<git_tree, ScopedPointerCustomDeleter> GitTreeFreeOnDestruct;
#if 0
{
public:
    GitTreeFreeOnDestruct(/*git_tree *treeToFree*/)
        : m_TreeToFree(NULL)
    { }
    void reset(git_tree *treeToFree = NULL, bool freeCurrentlyWatched = true)
    {
        if(freeCurrentlyWatched && m_TreeToFree)
        {
            git_tree_free(m_TreeToFree);
        }
        m_TreeToFree = treeToFree;
    }
    ~GitTreeFreeOnDestruct()
    {
        if(m_TreeToFree)
            git_tree_free(m_TreeToFree);
    }
private:
    git_tree *m_TreeToFree;
};
#endif
struct SortedMapOfListsOfPathsPointerTypeDeleter
{
    static inline void cleanup(SortedMapOfListsOfPathsPointerType *sortedMap)
    {
        qDeleteAll(sortedMap->begin(), sortedMap->end());
        delete sortedMap;
    }
};
struct GitCommitFreeOnDestructDeleter
{
    static inline void cleanup(git_commit *commitToFree)
    {
        git_commit_free(commitToFree);
    }
};
typedef QScopedPointer<git_commit, GitCommitFreeOnDestructDeleter> GitCommitFreeOnDestruct;
#if 0
{
public:
    GitCommitFreeOnDestruct(git_commit *commitToFree)
        : m_CommitToFree(commitToFree)
    { }
    ~GitCommitFreeOnDestruct()
    {
        git_commit_free(m_CommitToFree);
    }
private:
    git_commit *m_CommitToFree;
};
#endif
class GitObjectFreeOnDestruct
{
public:
    GitObjectFreeOnDestruct(git_object *objectToFree)
        : m_ObjectToFree(objectToFree)
    { }
    ~GitObjectFreeOnDestruct()
    {
        git_object_free(m_ObjectToFree);
    }
private:
    git_object *m_ObjectToFree;
};
class GitThreadsShutdownOnDestruct
{
public:
    ~GitThreadsShutdownOnDestruct()
    {
        git_threads_shutdown();
    }
};
//#define INDEX_HTML_FIFO_CACHE_KEY QString("indexHtmlFifoCache")
#define INDEX_HTML_FIFO_CACHE_ARRAY_KEY "indexHtmlFifoCacheArray"
#define INDEX_HTML_FIFO_CACHE_PATH_KEY "indexHtmlFifoCachePath"
#define INDEX_HTML_FIFO_CACHE_HTML_KEY "indexHtmlFifoCacheHtml"
class IndexHtmlFifo : public QQueue<QPair<QString, QString> >
{
public:
    explicit IndexHtmlFifo()
        : QQueue<QPair<QString, QString> >()
    {
        QSettings settings;
        int size = settings.beginReadArray(INDEX_HTML_FIFO_CACHE_ARRAY_KEY);
        for(int i = 0; i < size; ++i)
        {
            settings.setArrayIndex(i);
            QPair<QString, QString> current;
            current.first = settings.value(INDEX_HTML_FIFO_CACHE_PATH_KEY).toString();
            current.second = settings.value(INDEX_HTML_FIFO_CACHE_HTML_KEY).toString();
            /*my*/enqueue(current); //TODOreq: might need to push_front() instead if they are backwards, unsure but I think it's right how it is
        }
    }
#if 0
    void loadCacheFromSettings()
    {
        //if this was in constructor (as originally written), then wouldn't it be an infinite loop as QVariant tries to instantiate it?
        //QSettings settings;
        //*this = settings.value(INDEX_HTML_FIFO_CACHE_KEY).value<IndexHtmlFifo>(); //this line looks funky, not sure it'll work. almost certain it doesn't work in the constructor
    }
#endif
    void removeModificationEntriesByFilePath(const QString &filePath)
    {
        QMutableListIterator<QPair<QString, QString> > queueIterator(*this);
        while(queueIterator.hasNext())
        {
            QPair<QString, QString> entry = queueIterator.next();
            if(entry.first == filePath //the entry matches...
                    &&
                    entry.second.contains(A_STRING_IN_MODIFIED_FILE_HTML_KEK_HACK)) //..and it is a "modified the file" entry (we don't remove "created the file" entries")
            {
                queueIterator.remove();
                break;
            }
        }
    }
    void myEnqueue(QPair<QString, QString> newEntry)
    {
        //first filter out last modified timestamps file. it changes with every commit, so showing it [at the top] every time would be stupid
        if(newEntry.first == LAST_MODIFIED_TIMESTAMPS_FILEPATH)
            return;

        //then dedupe the new entry
        removeModificationEntriesByFilePath(newEntry.first);

        //add the new entry
        QQueue::enqueue(newEntry);

        //purge old entries
        while(size() > MAX_INDEXHTML_ENTRIES)
        {
            dequeue();
        }
    }
    void updateSecondWithoutMovingPositionInQueue(QPair<QString, QString> newSecond)
    {
        int siz3 = size();
        for(int i = 0; i < siz3; ++i)
        {
            QPair<QString, QString> current = at(i);
            if(current.first == newSecond.first)
            {
                replace(i, newSecond);
                return;
            }
        }
    }
    ~IndexHtmlFifo()
    {
        QSettings settings;
        QListIterator<QPair<QString, QString> > it(*this);
        settings.beginWriteArray(INDEX_HTML_FIFO_CACHE_ARRAY_KEY);
        int i = 0;
        while(it.hasNext())
        {
            settings.setArrayIndex(i);
            QPair<QString, QString> current = it.next();
            settings.setValue(INDEX_HTML_FIFO_CACHE_PATH_KEY, current.first);
            settings.setValue(INDEX_HTML_FIFO_CACHE_HTML_KEY, current.second);
            ++i;
        }
        settings.endArray();
    }
};
#define ARCHIVE_MONTH_HTML_ENTRY_MAP_TYPES qint64 /*timestamp*/, QString /*path*/
class ArchiveYearHtml;
class ArchiveMonthHtml
{
public:
    explicit ArchiveMonthHtml(ArchiveYearHtml *parentArchiveYearHtml, int month);
    //bool isStale() const { return m_Stale; }
    void addEntry(qint64 timestamp, const QString &path);
    void removeEntryByPath(const QString &path);
    ArchiveYearHtml *parentArchiveYear() const;
    int month() const;
    QMultiMap<ARCHIVE_MONTH_HTML_ENTRY_MAP_TYPES> entries() const;
private:
    //bool m_Stale;
    ArchiveYearHtml *m_ParentArchiveYear;
    int m_Month;
    QMultiMap<ARCHIVE_MONTH_HTML_ENTRY_MAP_TYPES> m_Entries;
};
typedef QMap<MONTHS_ARCHIVE_HTMLS_MAP_TYPES > MonthsArchiveHtmlsType;
class ArchiveYearHtml
{
public:
    explicit ArchiveYearHtml(ArchiveHtmls *parentArchiveHtmls, int year);
    ArchiveMonthHtml *getOrCreateMonthArchiveHtmlByMonth(int month);
    ArchiveHtmls *parentArchiveHtmls() const;
    int year() const;
    QMap<MONTHS_ARCHIVE_HTMLS_MAP_TYPES > monthEntries() const { return m_MonthsArchiveHtmls; }
    void removeMonth(ArchiveMonthHtml *month);
private:
    ArchiveHtmls *m_ParentArchiveHtmls;
    int m_Year;
    MonthsArchiveHtmlsType m_MonthsArchiveHtmls;
};
class ArchiveHtmlCreatedFileEntryType
{
public:
    ArchiveHtmlCreatedFileEntryType(qint64 timestamp, const QString &path)
        : Timestamp(timestamp)
        , Path(path)
    { }
    qint64 Timestamp;
    QString Path;
};
class ArchiveHtmlModifiedFileEntryType
{
public:
    ArchiveHtmlModifiedFileEntryType(qint64 oldTimestamp, qint64 newTimestamp, const QString &path)
        : OldTimestamp(oldTimestamp)
        , NewTimestamp(newTimestamp)
        , Path(path)
    { }
    qint64 OldTimestamp;
    qint64 NewTimestamp;
    QString Path;
};

class ArchiveHtmlDeletedFileEntryType
{
public:
    ArchiveHtmlDeletedFileEntryType(int year, int month, const QString &path)
        : Year(year)
        , Month(month)
        , Path(path)
    { }
    int Year;
    int Month;
    QString Path;
};
#define YEARS_ARCHIVE_HTMLS_TYPES int /*year*/, QSharedPointer<ArchiveYearHtml>
typedef QMap<YEARS_ARCHIVE_HTMLS_TYPES > YearsArchiveHtmlsType;
class ArchiveHtmls
{
public:
    ArchiveHtmls()
        : m_Stale(false)
    { }
    void addCreatedFileEntry(const ArchiveHtmlCreatedFileEntryType &createdFileEntry);
    void addModifiedFileEntry(const ArchiveHtmlModifiedFileEntryType &modifiedFileEntry);
    void addDeletedFileEntry(const ArchiveHtmlDeletedFileEntryType &deletedFileEntry);
    ArchiveYearHtml *getOrCreateYearArchiveHtmlByYear(int year);
    void applyMutations();
    QMap<YEARS_ARCHIVE_HTMLS_TYPES > yearsArchiveHtmls() const { return m_YearsArchiveHtmls; }
    void removeYear(ArchiveYearHtml *yearToRemove);

    bool isStale() const { return m_Stale; }
    void setStale(bool stale) { m_Stale = stale; }
    QSet<ArchiveYearHtml*> staleYears() const;
    void clearStaleYears() { m_StaleYears.clear(); }
    QSet<ArchiveMonthHtml *> staleMonths() const;
    void clearStaleMonths() { m_StaleMonths.clear(); }
private:
    friend class ArchiveYearHtml;
    friend class ArchiveMonthHtml;

    QList<ArchiveHtmlCreatedFileEntryType> m_CreatedFileEntries;
    QList<ArchiveHtmlModifiedFileEntryType> m_ModifiedFileEntries;
    QList<ArchiveHtmlDeletedFileEntryType> m_DeletedFileEntries;

    YearsArchiveHtmlsType m_YearsArchiveHtmls;

    bool m_Stale;
    QSet<ArchiveYearHtml*> m_StaleYears;
    QSet<ArchiveMonthHtml*> m_StaleMonths;

    void addStaleYear(ArchiveYearHtml *staleYear);
    void addStaleMonth(ArchiveMonthHtml *staleMonth);
};

#endif // QUICKDIRTYSTATICGITWEB_H
