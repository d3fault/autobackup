#include "symboliclinksdetector.h"

SymbolicLinksDetector::SymbolicLinksDetector(QObject *parent) :
    QObject(parent), m_QuitFlag(false)
{
    m_FiltersForDetectingSymbolicLinks |= QDir::Dirs;
    m_FiltersForDetectingSymbolicLinks |= QDir::Files;
    m_FiltersForDetectingSymbolicLinks |= QDir::NoDotAndDotDot;
    m_FiltersForDetectingSymbolicLinks |= QDir::Hidden;
    m_FiltersForDetectingSymbolicLinks |= QDir::System; //When copying and shit in other apps I generally don't have System... but for this test I want it because it will show me broken symlinks according to the docs...
}
void SymbolicLinksDetector::detectSymbolicLinks(QDir &searchDirectory, bool quitOnFirstFound_TRUE__OR__listEachOnesPath_FALSE)
{
    m_QuitOnFirstDetected = quitOnFirstFound_TRUE__OR__listEachOnesPath_FALSE;
    m_QuitFlag = false;

    if(!searchDirectory.exists())
    {
        emit d("Directory Does Not Exist (so no symbolic links within it lol)");
        return;
    }

    searchDirectory.setFilter(m_FiltersForDetectingSymbolicLinks);

    QFileInfoList searchDirectoryFileInfoList = searchDirectory.entryInfoList();

    emit d("Entering Recursive Detect Symbolic Links Method...");

    detectSymbolicLinksRecursingIntoDirs(searchDirectoryFileInfoList);

    emit d("...Exitted Recursive Detect Symbolic Links Method");
}
void SymbolicLinksDetector::detectSymbolicLinksRecursingIntoDirs(const QFileInfoList &currentDirectoryTree)
{
    QListIterator<QFileInfo> it(currentDirectoryTree);
    while(it.hasNext())
    {
        if(m_QuitFlag)
        {
            return;
        }

        m_CurrentFileInfo = it.next();

        //since .isDir will return true on symlinks, we want to check THAT first
        if(m_CurrentFileInfo.isSymLink())
        {
            QString resultDetails(QString("Found A Symbolic Link At: '") + m_CurrentFileInfo.filePath() + QString("'' -- which points to: '") + m_CurrentFileInfo.symLinkTarget() + QString("' -- and here is whether or not the target exists: ") + (m_CurrentFileInfo.exists() ? QString("Yes") : QString("No")));
            emit d(resultDetails);

            if(m_QuitOnFirstDetected)
            {
                m_QuitFlag = true; //gah recursion is a bitch, but this will be decent practice for the more complex "copy recursively" app
                return;
            }
        }
        else if(m_CurrentFileInfo.isDir()) //we don't want to go into symlinks... only report them... hence the "else if"
        {
            QDir nextSourceDir(m_CurrentFileInfo.canonicalFilePath());
            nextSourceDir.setFilter(m_FiltersForDetectingSymbolicLinks);

            QFileInfoList nextFileInfoList = nextSourceDir.entryInfoList();
            detectSymbolicLinksRecursingIntoDirs(nextFileInfoList);
        }
    }
}
