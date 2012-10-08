#include "easytree.h"

EasyTree::EasyTree(QObject *parent) :
    QObject(parent), m_Colon(":"), m_DirNamesToIgnore(0), m_FileNamesToIgnore(0), m_FileNamesEndWithIgnoreList(0)
{
    m_EscapedColon.append("\\"); //lol have to escape my escaper!
    m_EscapedColon.append(m_Colon);
}
EasyTree::~EasyTree()
{
    delete m_TreeTextStream;
    delete m_TreeText;
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

    m_CreatedDateTime = m_CurrentFileInfo.created();
    m_ModifiedDateTime = m_CurrentFileInfo.lastModified();

    //stream a colon, the file size, another colon, the creation date, another colon, the last modified date, and lastly a newline
    *m_TreeTextStream << m_Colon << QString::number(m_CreatedDateTime.toMSecsSinceEpoch()/1000) << m_Colon << QString::number(m_ModifiedDateTime.toMSecsSinceEpoch()/1000) << endl;
}
void EasyTree::addFileEntry()
{
    //file entries do not have a trailing slash and do specify a size in bytes just after the filename

    //stream the path and make it relative to the dir we are treeing by chopping that portion out of the abolute
    *m_TreeTextStream << m_CurrentFileInfo.canonicalFilePath().remove(0, m_DirWeAreTreeingChopLength).replace(m_Colon, m_EscapedColon, Qt::CaseSensitive); //i pass absoluteDirs from each call, and that chop() is how i hackily convert them into relative paths :)
    m_CreatedDateTime = m_CurrentFileInfo.created();
    m_ModifiedDateTime = m_CurrentFileInfo.lastModified();

    //stream a colon, the file size, another colon, the creation date, another colon, the last modified date, and lastly a newline
    *m_TreeTextStream << m_Colon << QString::number(m_CurrentFileInfo.size()) << m_Colon << QString::number(m_CreatedDateTime.toMSecsSinceEpoch()/1000) << m_Colon << QString::number(m_ModifiedDateTime.toMSecsSinceEpoch()/1000) << endl;
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
void EasyTree::generateTreeText(const QString &absoluteDirString, QList<QString> *dirNamesToIgnore, QList<QString> *fileNamesToIgnore, QList<QString> *fileNamesEndWithIgnoreList, QList<QString> *dirNamesEndsWithIgnoreList)
{
    m_DirWeAreTreeing = absoluteDirString;
    if(!m_DirWeAreTreeing.endsWith("/"))
    {
        m_DirWeAreTreeing.append("/");
    }
    m_DirWeAreTreeingChopLength = m_DirWeAreTreeing.length();
    m_TreeText->clear();
    m_TreeTextStream->setString(m_TreeText, QIODevice::WriteOnly | QIODevice::Text);
    m_TreeTextStream->seek(0);

    QFileInfo dirFileInfo(m_DirWeAreTreeing);
    if(!dirFileInfo.exists() || !dirFileInfo.isDir())
    {
        return;
    }

    //captured to keep recursive function arguments to a minimum (stack grows explosion)
    m_DirNamesToIgnore = dirNamesToIgnore;
    m_FileNamesToIgnore = fileNamesToIgnore;
    m_FileNamesEndWithIgnoreList = fileNamesEndWithIgnoreList;
    m_DirNamesEndWithIgnoreList = dirNamesEndsWithIgnoreList;

    //debug/test
    m_FileNamesToIgnore->append("quick.dirty.auto.backup.halper.generated.dir.structure.txt");
    m_FileNamesEndWithIgnoreList->append("~");
    m_FileNamesEndWithIgnoreList->append(".pro.user");
    m_FileNamesEndWithIgnoreList->append("2223");
    m_DirNamesToIgnore->append(".git");
    m_DirNamesEndWithIgnoreList->append("xD_Release");


    insertAllFileAndDirInfoIntoTreeTextRecursingIntoDirs(m_DirWeAreTreeing); //recursive function

    emit treeTextGenerated(*m_TreeText);
}
void EasyTree::initialize()
{
    m_TreeText = new QString();
    m_TreeTextStream = new QTextStream();
    m_Dir = new QDir();

    m_Dir->setSorting(QDir::DirsFirst | QDir::Name);
    QDir::Filters filters;
    filters |= QDir::Dirs;
    filters |= QDir::Files;
    filters |= QDir::NoSymLinks;
    filters |= QDir::NoDotAndDotDot;
    filters |= QDir::Readable;
    filters |= QDir::Hidden;
    m_Dir->setFilter(filters);
}
