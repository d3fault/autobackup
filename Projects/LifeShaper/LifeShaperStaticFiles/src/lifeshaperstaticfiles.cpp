#include "lifeshaperstaticfiles.h"

LifeShaperStaticFiles::LifeShaperStaticFiles(QObject *parent) :
    QObject(parent), m_FirstTimeInProcessNextLineForRecursion(true), m_LeaveBehindFilename("leave.behind.files.txt"), m_IffyCopyrightFilename("iffy.copyright.files.txt"), m_DeferDecisionFilename("defer.decision.files.txt"), m_BringForwardFilename("bring.forward.files.txt"), m_UseTHISasReplacementFilename("use.this.as.replacement.files.txt"), m_CurrentEasyTreeHashItem(0)
{ }
LifeShaperStaticFiles::~LifeShaperStaticFiles()
{
    cleanUp();
}
void LifeShaperStaticFiles::startIteratingEasyTreeHashFile(QIODevice *easyTreeHashFile, QString outputFilesFolder, QString lineLeftOffFrom_OR_emptyString)
{
    QFileInfo outputFilesFolderFileInfo(outputFilesFolder);
    if(!outputFilesFolderFileInfo.exists())
    {
        emit d("your output directory does not exist, stopping");
        return;
    }

    m_EasyTreeHashTextStream.setDevice(easyTreeHashFile);

    if(!lineLeftOffFrom_OR_emptyString.trimmed().isEmpty())
    {
        if(!outputFilesExistInFolder(outputFilesFolder))
        {
            emit d("some or all of your output files don't exist, so we cannot resume");
            return;
        }
        else
        {
            //open them for appending
            m_LeaveBehindFile.setFileName(outputFilesFolder + m_LeaveBehindFilename);
            if(!m_LeaveBehindFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
            {
                emit d("failed to open leave behind file for appending");
                return;
            }

            m_IffyCopyrightFile.setFileName(outputFilesFolder + m_IffyCopyrightFilename);
            if(!m_IffyCopyrightFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
            {
                emit d("failed to open iffy copyright file for appending");
                return;
            }

            m_DeferDecisionFile.setFileName(outputFilesFolder + m_DeferDecisionFilename);
            if(!m_DeferDecisionFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
            {
                emit d("failed to open defer decision file for appending");
                return;
            }

            m_BringForwardFile.setFileName(outputFilesFolder + m_BringForwardFilename);
            if(!m_BringForwardFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
            {
                emit d("failed to open bring forward file for appending");
                return;
            }

            m_UseTHISasReplacementFile.setFileName(outputFilesFolder + m_UseTHISasReplacementFilename);
            if(!m_UseTHISasReplacementFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
            {
                emit d("failed to open 'use this as replacement' file for appending");
                return;
            }
        }

        //the line isn't empty, so we try to find where we left off
        bool foundWhereLeftOff = false;

        while(m_EasyTreeHashTextStream.atEnd())
        {
            m_CurrentLine = m_EasyTreeHashTextStream.readLine();
            if(m_CurrentLine == lineLeftOffFrom_OR_emptyString)
            {
                foundWhereLeftOff = true; //we found it and m_CurrentLine is set to it!
                break;
            }
        }
        if(!foundWhereLeftOff)
        {
            emit d("you supplied a line left off at, but we couldn't find it in the easy tree hash file");
            return; //an error imo
        }
    }
    else
    {
        if(outputFilesExistInFolder(outputFilesFolder))
        {
            emit d("your output files already exist and you didn't supply a point left off from, stopping");
            return;
        }
        else
        {
            //create/open them
            m_LeaveBehindFile.setFileName(outputFilesFolder + m_LeaveBehindFilename);
            if(!m_LeaveBehindFile.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                emit d("failed to open leave behind file for writing");
                return;
            }

            m_IffyCopyrightFile.setFileName(outputFilesFolder + m_IffyCopyrightFilename);
            if(!m_IffyCopyrightFile.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                emit d("failed to open iffy copyright file for writing");
                return;
            }

            m_DeferDecisionFile.setFileName(outputFilesFolder + m_DeferDecisionFilename);
            if(!m_DeferDecisionFile.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                emit d("failed to open defer decision file for writing");
                return;
            }

            m_BringForwardFile.setFileName(outputFilesFolder + m_BringForwardFilename);
            if(!m_BringForwardFile.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                emit d("failed to open bring forward file for writing");
                return;
            }

            m_UseTHISasReplacementFile.setFileName(outputFilesFolder + m_UseTHISasReplacementFilename);
            if(!m_UseTHISasReplacementFile.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                emit d("failed to open 'use this as replacement' file for writing");
                return;
            }
        }

        //just read the first line so we're in sync with the above if statement
        if(!m_EasyTreeHashTextStream.atEnd())
        {
            m_CurrentLine = m_EasyTreeHashTextStream.readLine();
        }
        else
        {
            emit d("size zero input file");
            return;
        }
    }

    m_LeaveBehindTextStream.setDevice(&m_LeaveBehindFile);
    m_IffyCopyrightTextStream.setDevice(&m_IffyCopyrightFile);
    m_DeferDecisionTextStream.setDevice(&m_DeferDecisionFile);
    m_BringForwardTextStream.setDevice(&m_BringForwardFile);
    m_UseTHISasReplacementTextStream.setDevice(&m_UseTHISasReplacementFile);

    //at this point m_CurrentLine is populated with the last read line and m_EasyTreeHashTextStream is ready to read the next line
    processCurrentEasyTreeHashLineRegularly();
}
void LifeShaperStaticFiles::stopIteratingEasyTreeHashFile()
{
    emit d(QString("make sure you save the current line somewhere if you want to resume later: ") + m_CurrentLine);
    cleanUp();
}
void LifeShaperStaticFiles::setRecursivelyApplyFromHereUntilParent(bool recursiveEnabled)
{
    m_RecursiveUntilParent = recursiveEnabled;
}
void LifeShaperStaticFiles::leaveBehind()
{
    m_LeaveBehindTextStream << m_CurrentLine << endl;
    emit d(QString("leaving behind: ") + m_CurrentEasyTreeHashItem->relativeFilePath());
    if(m_RecursiveUntilParent)
    {
        m_TextStreamPointerForRecursionHacksLoL = &m_LeaveBehindTextStream;
        m_SpecialCaseForUseTHIStextStreamAndOnlyForDirs = false;
        m_FirstTimeInProcessNextLineForRecursion = true;
    }
    processNextLineOrNotifyOfCompletion();
}
void LifeShaperStaticFiles::iffyCopyright()
{
    m_IffyCopyrightTextStream << m_CurrentLine << endl;
    emit d(QString("iffy copyright: ") + m_CurrentEasyTreeHashItem->relativeFilePath());
    if(m_RecursiveUntilParent)
    {
        m_TextStreamPointerForRecursionHacksLoL = &m_IffyCopyrightTextStream;
        m_SpecialCaseForUseTHIStextStreamAndOnlyForDirs = false;
        m_FirstTimeInProcessNextLineForRecursion = true;
    }
    processNextLineOrNotifyOfCompletion();
}
void LifeShaperStaticFiles::deferDecision()
{
    m_DeferDecisionTextStream << m_CurrentLine << endl;
    emit d(QString("deferring decision: ") + m_CurrentEasyTreeHashItem->relativeFilePath());
    if(m_RecursiveUntilParent)
    {
        m_TextStreamPointerForRecursionHacksLoL = &m_DeferDecisionTextStream;
        m_SpecialCaseForUseTHIStextStreamAndOnlyForDirs = false;
        m_FirstTimeInProcessNextLineForRecursion = true;
    }
    processNextLineOrNotifyOfCompletion();
}
void LifeShaperStaticFiles::bringForward()
{
    m_BringForwardTextStream << m_CurrentLine << endl;
    emit d(QString("bringing forward: ") + m_CurrentEasyTreeHashItem->relativeFilePath());
    if(m_RecursiveUntilParent)
    {
        m_TextStreamPointerForRecursionHacksLoL = &m_BringForwardTextStream;
        m_SpecialCaseForUseTHIStextStreamAndOnlyForDirs = false;
        m_FirstTimeInProcessNextLineForRecursion = true;
    }
    processNextLineOrNotifyOfCompletion();
}
void LifeShaperStaticFiles::useTHISasReplacement(QString replacementFilePath)
{
    //TODOreq: need new special format to account for 'this' replacement -- but we probably still want the whole m_CurrentLine along with it << endl;
    QString replacementFilePath2 = replacementFilePath;
    if(m_CurrentEasyTreeHashItem->isDirectory() && !replacementFilePath2.endsWith("/"))
    {
        replacementFilePath2.append("/");
    }
    m_UseTHISasReplacementTextStream << replacementFilePath2 << QString(":") << m_CurrentLine << endl;

    emit d(QString("using replacement for: ") + m_CurrentEasyTreeHashItem->relativeFilePath());
    if(m_CurrentEasyTreeHashItem->isDirectory())
    {
        m_RecursiveUntilParent = true;
    }
    else
    {
        //file
        if(m_RecursiveUntilParent)
        {
            emit d("can't recursively 'use this as replacement' on your file because you can only supply one replacement at a time -- but can on directories (none of the files will be listed and they all get the timestamp of the dir when copied/added)");
            m_RecursiveUntilParent = false;
            emit stoppedRecursingDatOperationBecauseParentDirEntered();
        }
    }
    if(m_RecursiveUntilParent) //because if it's a directory, we already handle it recursively in a sense -- for this op only
    {
        m_TextStreamPointerForRecursionHacksLoL = &m_UseTHISasReplacementTextStream;
        m_SpecialCaseForUseTHIStextStreamAndOnlyForDirs = true; //setting this to true tells our recursion logic to not print anything into the useThis dir after the dir itself. TODOreq: notify the user that all their timestamps will be lost and set to the timestamps of the dir. the user is expected to remake the dir structure as they see fit, so it doesn't make sense to keep file stats around
        m_FirstTimeInProcessNextLineForRecursion = true;
    }
    processNextLineOrNotifyOfCompletion();
}
void LifeShaperStaticFiles::processCurrentEasyTreeHashLineRegularly()
{
    if(m_CurrentEasyTreeHashItem)
    {
        //this if/delete isn't strictly necessary (we do it elsewhere), it's a just in case (just in cases never really exist in programming idfk why i have this here but it's leik extra safety (which, again, makes no sense at all) lawl)
        delete m_CurrentEasyTreeHashItem;
    }

    m_CurrentEasyTreeHashItem = EasyTreeHashItem::newEasyTreeHashItemFromLineOfText(m_CurrentLine, this);
    emit nowProcessingEasyTreeHashItem(m_CurrentLine, m_CurrentEasyTreeHashItem->relativeFilePath(), m_CurrentEasyTreeHashItem->isDirectory(), m_CurrentEasyTreeHashItem->isDirectory() ? 0 : m_CurrentEasyTreeHashItem->fileSize(), m_CurrentEasyTreeHashItem->creationDateTime().toString(), m_CurrentEasyTreeHashItem->lastModifiedDateTime().toString());
}
void LifeShaperStaticFiles::processNextLineOrNotifyOfCompletion()
{
    if(m_RecursiveUntilParent && m_FirstTimeInProcessNextLineForRecursion)
    {
        //save the directory of whatever we said to apply the given operation on for the rest of this dir and subdirs until parent

        if(m_CurrentEasyTreeHashItem->isDirectory())
        {
            m_RecursionStartsWithFilePathToDetectWhenParentComes = m_CurrentEasyTreeHashItem->relativeFilePath();
            if(!m_RecursionStartsWithFilePathToDetectWhenParentComes.endsWith("/"))
            {
                m_RecursionStartsWithFilePathToDetectWhenParentComes.append("/");
            }
        }
        else
        {
            QString currentRelativePath = m_CurrentEasyTreeHashItem->relativeFilePath();
            QList<QString> pathSegments = currentRelativePath.split("/");
            if(pathSegments.size() < 2)
            {
                emit d("cannot recurse the root directory you dolt -- mainly because we dunno when parent comes haha i suck"); //TODOoptional
                return;
            }
            QString filenameOnly = pathSegments.takeLast(); //take off filename portion

            QString reconstructedFilePathDirOnly = currentRelativePath.left(currentRelativePath.length() - filenameOnly.length());

            m_RecursionStartsWithFilePathToDetectWhenParentComes = reconstructedFilePathDirOnly;
        }
        m_FirstTimeInProcessNextLineForRecursion = false;
    }

    if(m_CurrentEasyTreeHashItem)
    {
        delete m_CurrentEasyTreeHashItem;
        m_CurrentEasyTreeHashItem = 0;
    }

    if(!m_EasyTreeHashTextStream.atEnd())
    {
        m_CurrentLine = m_EasyTreeHashTextStream.readLine();
        if(m_RecursiveUntilParent)
        {
            if(m_CurrentLine.startsWith(m_RecursionStartsWithFilePathToDetectWhenParentComes))
            {
                //recursion applies

                /*if(m_SpecialCaseForUseTHIStextStreamAndOnlyForDirs)
                {
                //if(m_UseThisAsReplacementChosenForDir)
                //{
                if(!m_WroteDirInUseThisSoDontWriteAnymore)
                {
                    //TO DOnereq: only doeet once!
                    appendReplacementLineToFile();
                }
                m_WroteDirInUseThisSoDontWriteAnymore = true;
                //}
                else
                {
                    //TO DOnereq: doeeet every time
                    //come to think of it, recursively "using this replacement for all rest files until parent" makes no fucking sense. zeus damnit. but i mean yea clicking 'use this replacement' on a dir is implicitly recursive and shouldn't write the files/folders within
                }
                }*/
                if(!m_SpecialCaseForUseTHIStextStreamAndOnlyForDirs) //the dir when you select 'use this replacement' has already been written, but we still need to recursively iterate and skip over the lines below it [until we get to parent]
                {
                    *m_TextStreamPointerForRecursionHacksLoL << m_CurrentLine << endl; //no need to parse it, just paste it!
                    emit d(QString("recursively adding: ") + m_CurrentLine + QString(" to whatever you selected -- we don't know at this layer lol"));
                }
                processNextLineOrNotifyOfCompletion(); //HOLY RECURSION BATMAN
            }
            else
            {
                //must have entered parent, disable recursion! we should also tell the GUI to un-check it's checkbox too!
                m_RecursiveUntilParent = false;
                m_FirstTimeInProcessNextLineForRecursion = true;
                m_SpecialCaseForUseTHIStextStreamAndOnlyForDirs = false;
                emit stoppedRecursingDatOperationBecauseParentDirEntered();
            }
        }
        else
        {
            processCurrentEasyTreeHashLineRegularly();
        }
    }
    else
    {
        emit finishedProcessingEasyTreeHashFile();
    }
}
bool LifeShaperStaticFiles::outputFilesExistInFolder(QString basePath)
{
    if(QFile::exists(basePath + m_LeaveBehindFilename))
    {
        if(QFile::exists(basePath + m_IffyCopyrightFilename))
        {
            if(QFile::exists(basePath + m_DeferDecisionFilename))
            {
                if(QFile::exists(basePath + m_BringForwardFilename))
                {
                    if(QFile::exists(basePath + m_UseTHISasReplacementFilename))
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
void LifeShaperStaticFiles::cleanUp()
{
    if(m_LeaveBehindFile.isOpen()) m_LeaveBehindFile.close();
    if(m_IffyCopyrightFile.isOpen()) m_IffyCopyrightFile.close();
    if(m_DeferDecisionFile.isOpen()) m_DeferDecisionFile.close();
    if(m_BringForwardFile.isOpen()) m_BringForwardFile.close();
    if(m_UseTHISasReplacementFile.isOpen()) m_UseTHISasReplacementFile.close();
    if(m_CurrentEasyTreeHashItem)
    {
        delete m_CurrentEasyTreeHashItem;
        m_CurrentEasyTreeHashItem = 0;
    }
    m_FirstTimeInProcessNextLineForRecursion = true;
}
