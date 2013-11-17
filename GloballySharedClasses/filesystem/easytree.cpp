#include "easytree.h"

/*
Notes for parsing (reading):
1) Colons in filenames/directory-names are allowed and are the only thing escaped (by a "\")
2) Directories do not put their size, since it's always 4kb -- which means there is one less colon field for them
3) Directories always append a slash at the end just to make them easier to differentiate (though #2 is enough tbh)
4) Creation Date is the first timestamp that appears, then Modified Date
5) Newlines indicate the end of a record. They are filesystem specific (\r\n on Windows for example) but Qt can handle both without any problems (if you use QTextStream correctly with the "Text" flag)!

6) Not yet: I am working on EasyTreeHasher that adds another field for md5/sha1 (detecting which will be done via the length)... so the hypothetical parser could parse both by seeing that there's +1 "colon field" for each dir/file type (and then it would be imperitive that we identify directories by the trailing slash... since the "directory entry with hash" (+1) will now have the same amount of "colon fields" as the "file entry without a hash". HOWEVER it doesn't make any fucking sense to have a hash for a directory (does it? only thing i could think of is hashing the entry list details after they've been concatenated... but wouldn't this be redundant? pretty sure yes but redundant but not 100% sure) so nevermind lmfao. However I'm not even sure it's wise to make them share a parser (I just know it's possible)
*/

const qint64 EasyTree::m_MaxReadSize = 4194304; //4mb (max) read buffer

EasyTree::EasyTree(QObject *parent) :
    QObject(parent), m_Colon(":"), m_TreeTextStream(0), m_DirNamesToIgnore(0), m_FileNamesToIgnore(0), m_FileNamesEndWithIgnoreList(0)
{
    m_EscapedColon.append("\\"); //lol have to escape my escaper!
    m_EscapedColon.append(m_Colon);


    //m_IODevice = new QString();
    m_TreeTextStream = new QTextStream();
    m_Dir = new QDir();

    m_Dir->setSorting(QDir::DirsFirst | QDir::Name);
    QDir::Filters filters;
    filters |= QDir::Dirs;
    filters |= QDir::Files;
    filters |= QDir::NoSymLinks;
    filters |= QDir::NoDotAndDotDot;
    filters |= QDir::Hidden;
    m_Dir->setFilter(filters);
}
EasyTree::~EasyTree()
{
    delete m_TreeTextStream;
    //delete m_IODevice;
    delete m_Dir;
}
void EasyTree::insertAllFileAndDirInfoIntoTreeTextRecursingIntoDirs(const QString &absoluteDirString)
{
    m_Dir->setPath(absoluteDirString);
    QFileInfoList dirFileInfos = m_Dir->entryInfoList();
    QListIterator<QFileInfo> it(dirFileInfos);
    while(it.hasNext())
    {
        m_CurrentFileInfo = it.next();

        if(m_CurrentFileInfo.isDir())
        {
            if(weDontWantToSkipCurrentDirInfo())
            {
                addDirEntry();

                //call self recursively
                insertAllFileAndDirInfoIntoTreeTextRecursingIntoDirs(m_CurrentFileInfo.canonicalFilePath());
            }
        }
        else //file
        {
            if(weDontWantToSkipCurrentFileInfo())
            {
                addFileEntry();
            }
        }
    }
}
void EasyTree::addDirEntry()
{
    //dir entries having a trailing slash and do not specify size as it is always 4096

    //stream the path and make it relative to the dir we are treeing by chopping that portion out of the abolute
    //also escape colons which are apparently legal in fucking filenames...
    *m_TreeTextStream << m_CurrentFileInfo.canonicalFilePath().remove(0, m_DirWeAreTreeingChopLength).replace(m_Colon, m_EscapedColon, Qt::CaseSensitive) << "/"; //dirs are identified by trailing slashes in my EasyTree, so add it. canonical path does not return a slash

    if(m_UseObsoleteFormatWithSizeAndCreationDate)
    {
        m_CreatedDateTime = m_CurrentFileInfo.created();
        *m_TreeTextStream << m_Colon << QString::number(m_CreatedDateTime.toMSecsSinceEpoch()/1000);
    }

    m_ModifiedDateTime = m_CurrentFileInfo.lastModified();
    //stream a colon, the file size, another colon, the creation date, another colon, the last modified date, and lastly a newline
     *m_TreeTextStream << m_Colon << QString::number(m_ModifiedDateTime.toMSecsSinceEpoch()/1000) << endl;
}
void EasyTree::addFileEntry()
{
    //file entries do not have a trailing slash and do specify a size in bytes just after the filename

    //stream the path and make it relative to the dir we are treeing by chopping that portion out of the abolute
    *m_TreeTextStream << m_CurrentFileInfo.canonicalFilePath().remove(0, m_DirWeAreTreeingChopLength).replace(m_Colon, m_EscapedColon, Qt::CaseSensitive); //i pass absoluteDirs from each call, and that chop() is how i hackily convert them into relative paths :)

    m_ModifiedDateTime = m_CurrentFileInfo.lastModified();

    //stream a colon, the file size, another colon, the creation date, another colon, the last modified date, and lastly a newline

    if(m_UseObsoleteFormatWithSizeAndCreationDate)
    {
        m_CreatedDateTime = m_CurrentFileInfo.created();
        *m_TreeTextStream << m_Colon << QString::number(m_CurrentFileInfo.size()) << m_Colon << QString::number(m_CreatedDateTime.toMSecsSinceEpoch()/1000);
    }

    *m_TreeTextStream << m_Colon << QString::number(m_ModifiedDateTime.toMSecsSinceEpoch()/1000);

    if(m_CalculateMd5Sums)
    {
        //code to "hash a file" jacked from RecursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWay -- I should probably put it in a shared library. It isn't as easy as doing [static] hash(file.readAll()) because you would run out of memory on large files. This design uses a [4mb] buffer
        QCryptographicHash hasher(QCryptographicHash::Md5);

        QFile sourceFile2Hash(m_CurrentFileInfo.canonicalFilePath());
        if(sourceFile2Hash.open(QFile::ReadOnly))
        {
            qint64 bytesAvail = sourceFile2Hash.bytesAvailable();
            qint64 toRead;
            while(bytesAvail > 0)
            {
                toRead = qMin(m_MaxReadSize, bytesAvail);
                QByteArray readChunkArray = sourceFile2Hash.read(toRead);
                hasher.addData(readChunkArray);
                bytesAvail = sourceFile2Hash.bytesAvailable();
            }
            *m_TreeTextStream << m_Colon << hasher.result().toHex();
        }
        else
        {
            qWarning("failed to open file for hashing"); //Eh I guess if we fail to open for reading, we just won't have a hash for that entry. Was tempted to use qFatal but then there'd be no cleanup etc :(. TODOreq: what's worse? inconsistent (psbly invalid) data, or segfaults/memory-leaks/etc. Also note that this particular example might not even cause anything that bad to happen if I used qFatal... but I mean IN GENERAL which is worse?
        }
    }

    *m_TreeTextStream << endl;
}
bool EasyTree::weDontWantToSkipCurrentFileInfo()
{
    int i;
    QString currentFilenameOnly = m_CurrentFileInfo.canonicalFilePath().split("/",QString::SkipEmptyParts).last();
    int listSize = m_FileNamesToIgnore->size();
    for(i = 0; i < listSize; ++i)
    {
        if(currentFilenameOnly == m_FileNamesToIgnore->at(i))
        {
            return false;
        }
    }
    listSize = m_FileNamesEndWithIgnoreList->size();
    for(i = 0; i < listSize; ++i)
    {
        if(currentFilenameOnly.endsWith(m_FileNamesEndWithIgnoreList->at(i)))
        {
            return false;
        }
    }
    return true;
}
bool EasyTree::weDontWantToSkipCurrentDirInfo()
{
    int i;
    QString currentFilenameOnly = m_CurrentFileInfo.canonicalFilePath().split("/",QString::SkipEmptyParts).last();
    int listSize = m_DirNamesToIgnore->size();
    for(i = 0; i < listSize; ++i)
    {
        if(currentFilenameOnly == m_DirNamesToIgnore->at(i))
        {
            return false;
        }
    }
    listSize = m_DirNamesEndWithIgnoreList->size();
    for(i = 0; i < listSize; ++i)
    {
        if(currentFilenameOnly.endsWith(m_DirNamesEndWithIgnoreList->at(i)))
        {
            return false;
        }
    }
    return true;
}
void EasyTree::generateTreeText(const QString &absoluteDirString, QIODevice *ioDeviceToWriteTo, bool calculateMd5Sums, QList<QString> *dirNamesToIgnore, QList<QString> *fileNamesToIgnore, QList<QString> *fileNamesEndWithIgnoreList, QList<QString> *dirNamesEndsWithIgnoreList, bool useObsoleteFormatWithSizeAndCreationDate)
{
    m_UseObsoleteFormatWithSizeAndCreationDate = useObsoleteFormatWithSizeAndCreationDate;

    m_DirWeAreTreeing = absoluteDirString;
    if(!m_DirWeAreTreeing.endsWith("/"))
    {
        m_DirWeAreTreeing.append("/");
    }
    m_DirWeAreTreeingChopLength = m_DirWeAreTreeing.length();

    //m_IODevice->clear();
    m_TreeTextStream->setDevice(ioDeviceToWriteTo);
    //m_TreeTextStream->setString(m_IODevice, QIODevice::WriteOnly | QIODevice::Text);
    if(!ioDeviceToWriteTo->isSequential())
    {
        m_TreeTextStream->seek(0);
    }

    QFileInfo dirFileInfo(m_DirWeAreTreeing);
    if(!dirFileInfo.exists() || !dirFileInfo.isDir()) //TODOreq: ||  !dirFileInfo.isReadable() ?? had mixed results when using the QDir::Readable filter flag but I guess this isn't the exact same. This should ideally be done on each file... but wait a tick can't we still get info on a file we don't have read access to? Oh fuck isn't the answer to that OS specific? Like on one of the OS's then you need the directory to be readable in order to get the attributes for the file entry list, regardless of whether you can read the contents of those files? And on another OS it's.... ????? based on the file itself ????? _IDFK_. I guess... when in doubt.... run as root <3 (FAMOUS FUCKING LAST WORDS BECAUSE HE OVERWROTE HIS SYSTEM PARTITION AND/OR YEARS OF CODE/WORK LAWLAWLAWLAWLAWL. dats why it's a req... I need to figure it the fuck out)
    {
        return;
    }

    //captured to keep recursive function arguments to a minimum (stack grows explosion)
    m_CalculateMd5Sums = calculateMd5Sums;
    m_DirNamesToIgnore = dirNamesToIgnore;
    m_FileNamesToIgnore = fileNamesToIgnore;
    m_FileNamesEndWithIgnoreList = fileNamesEndWithIgnoreList;
    m_DirNamesEndWithIgnoreList = dirNamesEndsWithIgnoreList;

    insertAllFileAndDirInfoIntoTreeTextRecursingIntoDirs(m_DirWeAreTreeing); //recursive function

    //emit treeTextGenerated(*m_IODevice);
    //returning means we're done
}
