#include "copyandmergeaneasytreefileandareplacementeasytreefilewhileverifyingrelevantfiles.h"

CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFiles::CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFiles(QObject *parent) :
    QObject(parent)
{
}
void CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFiles::copyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFiles(const QString &regularEasyTreeFileName, const QString &replacementFormattedEasyTreeFileName, const QString &sourceBaseDirCorrespondingToEasyTreeFile, const QString &destinationDirToCopyTo, const QString &outputEasyTreeFileName)
{
    if(!QFile::exists(regularEasyTreeFileName))
    {
        emit d("Regular Easy Tree File Does Not Exist");
        return;
    }
    if(!QFile::exists(replacementFormattedEasyTreeFileName))
    {
        emit d("Replacements Easy Tree File Does Not Exist");
        return;
    }
    if(!QFile::exists(sourceBaseDirCorrespondingToEasyTreeFile))
    {
        emit d("Source Base Dir Does Not Exist");
        return;
    }
    if(!QFile::exists(destinationDirToCopyTo))
    {
        emit d("Destination Dir To Copy To Does Not Exist");
        return;
    }
    if(QFile::exists(outputEasyTreeFileName))
    {
        emit d("Output Easy Tree File Already Exists -- As a safety measure we refuse to overwrite it"); //ehh fuck it
        return;
    }

    QFile regularEasyTreeFile(regularEasyTreeFileName);
    if(!regularEasyTreeFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit d("Failed to open regular easy tree file for reading");
        return;
    }

    QFile replacementsEasyTreeFile(replacementFormattedEasyTreeFileName);
    if(!replacementsEasyTreeFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit d("Failed to open replacements easy tree file for reading");
        return;
    }

    QFile outputEasyTreeFile(replacementFormattedEasyTreeFileName);
    if(!outputEasyTreeFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit d("Failed to open output easy tree file for writing");
        return;
    }

    copyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFiles(&regularEasyTreeFile, &replacementsEasyTreeFile, sourceBaseDirCorrespondingToEasyTreeFile, destinationDirToCopyTo, &outputEasyTreeFile);
}
void CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFiles::copyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFiles(QIODevice *regularEasyTreeFile, QIODevice *replacementFormattedEasyTreeFile, const QString &sourceBaseDirCorrespondingToEasyTreeFile, const QString &destinationDirToCopyTo, QIODevice *outputEasyTreeFile)
{
    QString sourceBaseDirWithTrailingSlash = appendTrailingSlashIfMissing(sourceBaseDirCorrespondingToEasyTreeFile);
    QString destinationDirToCopyToWithTrailingSlash = appendTrailingSlashIfMissing(destinationDirToCopyTo);


    //read in each regular entry, copy + hash it to dest/output, compare hash with read in entry (report differences)
    //Damn me for not making EasyTreeHasher portable-er... but then really perhaps it would have been a premature optimization at the time! Hah! The programmer's [other] dilemma. The war in our brains of "portability vs. not-prematurely-optimizing". And that isn't portability in the OS sense, but in the "re-use in future projects" sense (and if you give your copyright away to your employer you're doubly fucked).

    //TO DOnereq: Directories and their entries are special case. I believe all the folder entries will be in "bring forward", regardless of if any of it's files are actually brought forward (some might be in copyright iffy, some might be left behind, etc). I am considering dropping folder entries altogether, however LifeShaperStaticFiles expects them when doing recursion (though does not require them IIRC so doing away with them might be ok (and am I really going to be running this output through that again!?!?)). One case where LifeShaperStaticFiles would behave drastically differently is if one folder has four subfolders (and no files). To recursively add that folder and it's four subfolders requires that we have a directory entry in the easy tree file. Otherwise it would ask for input on the first file it sees (likely in one of the four subfolders) and only be able to perform recursive operations from there (and not on the 3 neighbor folders (the problem is seen when the amount of neighbor folders is a few hundred/thousand)). In any case we need to make the full dir path up to where the file being copied should exist if it needs to be made.
    //There is also the issue of file entries perhaps coming before their dir entries (since I manually modified them). We might then have 2 folder entries if we generated one before TODOreq
    //On one hand I want to keep empty folder entries (the casualty of this decision), but on the other just forgetting about them makes the coding right now a lot easier. Example: what should the timestamps for a folder whose entry is not listed because it was in "bring forward" and not "iffy copyright" be when running "iffy copyright" through this app. The dir entry will be missing and therefore the timestamps will be too.
    //I will probably use a hacky QCache solution to see if a given directory has already been made during a single app execution (cache miss just does an actual dirExists syscall (and then populates cache after making if missing))... and this is regardless of if I decide to continue using dir entries.
    //I think the proper solution to this would have been to spit out the dir entries into any LifeShaper output that files are/were spit out into and to make sure that the dir entries always came before any sub-file entries (I manually modified some of the easy trees and can't guarantee that I kept them like that, but the EasyTreeHasher app does guarantee it). I never had that thought until just now :(. It's too late lol, I need a hacky solution ASEP and idgaf tbh.
    //Ok the hacky solution is staring me right in the face and isn't that complex: when a dir entry is seen, make the dir if it doesn't already exist (MIGHT if file entry in dir was above it) and add the entry to output tree file. When we get to a file where a dir doesn't yet exist, just make the dir but don't worry about adding the dir entry to the output easy tree. It's dir timestamp could be in another Easy Tree file or in some cases might come later in the file... so we'll just not worry about it (but making the dir on the actual FS is much more important).

    //TODOreq: verify sources for both regulars and replacements before copying. Maybe I should first code a "verify files existences" app... but really it mainly applies to replacements. I will probably be ok (famous last words, seeing as I'll be running this as root (inb4 aneurism)).

    QTextStream regularEasyTreeFileTextStream(regularEasyTreeFile);
    while(!regularEasyTreeFileTextStream->atEnd())
    {
        QString currentLine = regularEasyTreeFileTextStream.readLine();
        if(!currentLine.trimmed().isEmpty())
        {
            EasyTreeHashItem *easyTreeHashItem = EasyTreeHashItem::newEasyTreeHashItemFromLineOfText(currentLine);

            QFileInfo sourceFileInfo(sourceBaseDirWithTrailingSlash + easyTreeHashItem->relativeFilePath());
            if(!sourceFileInfo.exists())
            {
                emit d
            }

            EasyTreeHasher::copyAndHashSimultaneously
        }
    }


    //do same thing for replacements, but not comparing hashes since each replacement has a new hash
}
