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
        emit d("Destination Dir To Copy To Does Not Exist"); //TODOreq: verify it's empty, error out if it isn't
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

    QFile outputEasyTreeFile(outputEasyTreeFileName);
    if(!outputEasyTreeFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        emit d("Failed to open output easy tree file for writing");
        return;
    }

    copyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFiles(&regularEasyTreeFile, &replacementsEasyTreeFile, sourceBaseDirCorrespondingToEasyTreeFile, destinationDirToCopyTo, &outputEasyTreeFile);
}
void CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFiles::copyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFiles(QIODevice *regularEasyTreeFile, QIODevice *replacementFormattedEasyTreeFile, const QString &sourceBaseDirCorrespondingToEasyTreeFile, const QString &destinationDirToCopyToString, QIODevice *outputEasyTreeFile)
{
    QString sourceBaseDirWithTrailingSlash = appendTrailingSlashIfMissing(sourceBaseDirCorrespondingToEasyTreeFile);
    int sourceBaseDirWithTrailingSlashLength = sourceBaseDirWithTrailingSlash.length();
    QString destinationBaseDirToCopyToWithTrailingSlash = appendTrailingSlashIfMissing(destinationDirToCopyToString);

    QDir baseDestinationDirToCopyTo(destinationBaseDirToCopyToWithTrailingSlash);
    if(!baseDestinationDirToCopyTo.exists())
    {
        emit d("base destination dir to copy to does not exist");
        return;
    }

    //read in each regular entry, copy + hash it to dest/output, compare hash with read in entry (report differences)
    //Damn me for not making EasyTreeHasher portable-er... but then really perhaps it would have been a premature optimization at the time! Hah! The programmer's [other] dilemma. The war in our brains of "portability vs. not-prematurely-optimizing". And that isn't portability in the OS sense, but in the "re-use in future projects" sense (and if you give your copyright away to your employer you're doubly fucked).

    //TO DOnereq: Directories and their entries are special case. I believe all the folder entries will be in "bring forward", regardless of if any of it's files are actually brought forward (some might be in copyright iffy, some might be left behind, etc). I am considering dropping folder entries altogether, however LifeShaperStaticFiles expects them when doing recursion (though does not require them IIRC so doing away with them might be ok (and am I really going to be running this output through that again!?!?)). One case where LifeShaperStaticFiles would behave drastically differently is if one folder has four subfolders (and no files). To recursively add that folder and it's four subfolders requires that we have a directory entry in the easy tree file. Otherwise it would ask for input on the first file it sees (likely in one of the four subfolders) and only be able to perform recursive operations from there (and not on the 3 neighbor folders (the problem is seen when the amount of neighbor folders is a few hundred/thousand)). In any case we need to make the full dir path up to where the file being copied should exist if it needs to be made.
    //There is also the issue of file entries perhaps coming before their dir entries (since I manually modified them). We might then have 2 folder entries if we generated one before TO DOnereq
    //On one hand I want to keep empty folder entries (the casualty of this decision), but on the other just forgetting about them makes the coding right now a lot easier. Example: what should the timestamps for a folder whose entry is not listed because it was in "bring forward" and not "iffy copyright" be when running "iffy copyright" through this app. The dir entry will be missing and therefore the timestamps will be too.
    //I will probably use a hacky QCache solution to see if a given directory has already been made during a single app execution (cache miss just does an actual dirExists syscall (and then populates cache after making if missing))... and this is regardless of if I decide to continue using dir entries.
    //I think the proper solution to this would have been to spit out the dir entries into any LifeShaper output that files are/were spit out into and to make sure that the dir entries always came before any sub-file entries (I manually modified some of the easy trees and can't guarantee that I kept them like that, but the EasyTreeHasher app does guarantee it). I never had that thought until just now :(. It's too late lol, I need a hacky solution ASEP and idgaf tbh.
    //Ok the hacky solution is staring me right in the face and isn't that complex: when a dir entry is seen, make the dir if it doesn't already exist (MIGHT if file entry in dir was above it) and add the entry to output tree file. When we get to a file where a dir doesn't yet exist, just make the dir but don't worry about adding the dir entry to the output easy tree. It's dir timestamp could be in another Easy Tree file or in some cases might come later in the file... so we'll just not worry about it (but making the dir on the actual FS is much more important).

    //TO DOnereq: verify sources for both regulars and replacements before copying. Maybe I should first code a "verify files existences" app... but really it mainly applies to replacements. I will probably be ok (famous last words, seeing as I'll be running this as root (inb4 aneurism)).

    quint64 numberOfEntriesProcessed = 0.0;

    QTextStream regularEasyTreeFileTextStream(regularEasyTreeFile);
    QTextStream outputEasyTreeFileTextStream(outputEasyTreeFile);
    while(!regularEasyTreeFileTextStream.atEnd())
    {
        QString currentLine = regularEasyTreeFileTextStream.readLine();
        if(!currentLine.trimmed().isEmpty())
        {
            QScopedPointer<EasyTreeHashItem> parsedSourceEasyTreeHashItem(EasyTreeHashItem::newEasyTreeHashItemFromLineOfText(currentLine));

            //First check source actually exists
            QFileInfo sourceFileInfo(sourceBaseDirWithTrailingSlash + parsedSourceEasyTreeHashItem->relativeFilePath());
            if(!sourceFileInfo.exists())
            {
                emit d(QString("Failed to find a needed file in the source dir: ") + sourceFileInfo.canonicalFilePath());
                return;
            }

            QString absoluteDestinationFilePath(destinationBaseDirToCopyToWithTrailingSlash + parsedSourceEasyTreeHashItem->relativeFilePath());

            //Now process the entry, depending on if it's a directory or file
            if(parsedSourceEasyTreeHashItem->isDirectory())
            {
                //if the entry is a dir, make it if it doesn't exist (might already) and definitely add the entry to the output
                if(!QFile::exists(absoluteDestinationFilePath))
                {
                    QDir dirBeingMade(absoluteDestinationFilePath); //do I even need to pass this into the constructor? why isn't mkpath static!?
                    if(!dirBeingMade.mkpath(absoluteDestinationFilePath))
                    {
                        emit d(QString("failed to make path: ") + absoluteDestinationFilePath);
                        return;
                    }
                }

                //regardless of whether or not we needed to make a directory, we add an entry to the output
                outputEasyTreeFileTextStream << currentLine << endl; //no need to re-format it since the entries are identical
                ++numberOfEntriesProcessed;
            }
            else //file
            {
                int lastSeparatorIndex = absoluteDestinationFilePath.lastIndexOf("/", -1, Qt::CaseSensitive);
                if(lastSeparatorIndex <= -1)
                {
                    emit d("didn't find a single slash in absolute destination dir. wtf? absoluteDestinationFilePath:" + absoluteDestinationFilePath);
                    return;
                }
                QString absoluteDestinationDirStringIncludingTrailingSlash = absoluteDestinationFilePath.left(lastSeparatorIndex + 1);

                QDir absoluteDestinationDir(absoluteDestinationDirStringIncludingTrailingSlash);
                //first check that the file's dir exists, then make it if it doesn't. we don't bother writing about the dir to the output easy tree file though
                if(!absoluteDestinationDir.exists())
                {
                    if(!absoluteDestinationDir.mkpath(absoluteDestinationDirStringIncludingTrailingSlash))
                    {
                        emit d(QString("failed to make path: ") + absoluteDestinationDirStringIncludingTrailingSlash);
                        return;
                    }
                }

                EasyTreeHashItem *generatedDestinationEasyTreeHashItem = EasyTreeHasher::copyAndHashSimultaneously(sourceFileInfo, absoluteDestinationDir, QCryptographicHash::Md5 /*TODOoptional: newEasyTreeHashItemFromLineOfText should also set what algorithm is being used so we can auto select the algorithm to use during the copy. hard coding as md5 for now because lazy and KISS. Also, when I change it here I need to change it below for replacements as well*/, sourceBaseDirWithTrailingSlashLength);
                if(!generatedDestinationEasyTreeHashItem)
                {
                    emit d("generatedDestinationEasyTreeHashItem is null for regular entry, so there was an error during copying. check stderr");
                    return;
                }
                if(parsedSourceEasyTreeHashItem->hash() != generatedDestinationEasyTreeHashItem->hash())
                {
                    emit d(QString("checksum calcualted during copy (") + generatedDestinationEasyTreeHashItem->hash().toHex() + QString(") did not match the source's hash in easy tree input (") + parsedSourceEasyTreeHashItem->hash().toHex() + QString(")"));
                    //return; //to continue or to stop, that is the question...
                }

                outputEasyTreeFileTextStream << currentLine << endl; //again, no need to re-format it. only replacements need to call dest->toCommaSeparatedLineOfText, since they have a new hash
                ++numberOfEntriesProcessed;

                delete generatedDestinationEasyTreeHashItem;
            }
        }
    }




    //now do same thing for replacements, but not comparing hashes since each replacement has a new hash
    //do replacements have dir entries? for now: no
    QTextStream replacementFormattedEasyTreeFileTextStream(replacementFormattedEasyTreeFile);
    while(!replacementFormattedEasyTreeFileTextStream.atEnd())
    {
        QString currentLine = replacementFormattedEasyTreeFileTextStream.readLine();
        if(!currentLine.trimmed().isEmpty())
        {
            QScopedPointer<EasyTreeHashItem> parsedSourceReplacementEasyTreeHashItem(EasyTreeHashItem::newEasyTreeHashItemFromLineOfText(currentLine, true));

            //not a directory
            if(parsedSourceReplacementEasyTreeHashItem->isDirectory())
            {
                emit d("found a dir in the replacements formatted easy tree file, which is not allowed");
                return;
            }

            //source replacement exists
            QFileInfo sourceReplacementFileInfo(parsedSourceReplacementEasyTreeHashItem->absoluteReplacementFilePath());
            if(!sourceReplacementFileInfo.exists())
            {
                emit d(QString("the source replacement file (") + parsedSourceReplacementEasyTreeHashItem->absoluteReplacementFilePath() + QString(") does not exist for destination entry: (") + parsedSourceReplacementEasyTreeHashItem->relativeFilePath() + QString(")"));
                return;
            }

            QString absoluteDestinationFilePath(destinationBaseDirToCopyToWithTrailingSlash + parsedSourceReplacementEasyTreeHashItem->relativeFilePath());

            int lastSeparatorIndex = absoluteDestinationFilePath.lastIndexOf("/", -1, Qt::CaseSensitive);
            if(lastSeparatorIndex <= -1)
            {
                emit d("didn't find a single slash in absolute destination dir. wtf? absoluteDestinationFilePath:" + absoluteDestinationFilePath);
                return;
            }
            QString absoluteDestinationDirStringIncludingTrailingSlash = absoluteDestinationFilePath.left(lastSeparatorIndex + 1);

            QDir absoluteDestinationDir(absoluteDestinationDirStringIncludingTrailingSlash);

            //destination folder exists (make it if not (copy/paste from regular file entries above))
            if(!absoluteDestinationDir.exists())
            {
                if(!absoluteDestinationDir.mkpath(absoluteDestinationDirStringIncludingTrailingSlash))
                {
                    emit d(QString("failed to make path: ") + absoluteDestinationDirStringIncludingTrailingSlash);
                    return;
                }
            }

            //re-using variables and also getting the correct length for sourceBaseDirWithTrailingSlashLength before calling copyAndHashSimultaneously. each "replacement" can have a different source dir (therefore length), whereas "regular" entries all have the same so we don't need to re-calculate it every time
            //it is incorrect to send in the absolute replacement file path length as the final argument to copyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFiles, but it is safe. After the copy is complete we will fix the relativeFilePath to contain the correct relative path
            lastSeparatorIndex = parsedSourceReplacementEasyTreeHashItem->absoluteReplacementFilePath().lastIndexOf("/", -1, Qt::CaseSensitive);
            if(lastSeparatorIndex <= -1)
            {
                emit d("didn't find a single slash in replacement absolute file path. wtf? absoluteReplacementFilePath:" + parsedSourceReplacementEasyTreeHashItem->absoluteReplacementFilePath());
                return;
            }
            sourceBaseDirWithTrailingSlashLength = parsedSourceReplacementEasyTreeHashItem->absoluteReplacementFilePath().left(lastSeparatorIndex + 1).length();

            EasyTreeHashItem *generatedDestinationEasyTreeHashItem = EasyTreeHasher::copyAndHashSimultaneously(sourceReplacementFileInfo, absoluteDestinationDir, QCryptographicHash::Md5 /*see comment in regular file entry*/, sourceBaseDirWithTrailingSlashLength);
            if(!generatedDestinationEasyTreeHashItem)
            {
                emit d("generatedDestinationEasyTreeHashItem is null for replacement entry, so there was an error during copying. check stderr");
                return;
            }

            //As a hack, generatedDestinationEasyTreeHashItem->relativeFilePath() only has the filename portion of the entry. We need to hackily add back in the "relative dir"
            QString filenameOnly = generatedDestinationEasyTreeHashItem->relativeFilePath(); //has final/replacement filename [only]
            QString parsedOriginalRelativeFilePath = parsedSourceReplacementEasyTreeHashItem->relativeFilePath(); //has the relative path we need, but the old pre-replacement filename
            lastSeparatorIndex = parsedOriginalRelativeFilePath.lastIndexOf("/", -1, Qt::CaseSensitive);
            if(lastSeparatorIndex > -1)
            {
                //there is a slash somewhere
                QString relativeFolderPathWithTrailingSlash = parsedOriginalRelativeFilePath.left(lastSeparatorIndex + 1);
                //apply the hacky fix
                generatedDestinationEasyTreeHashItem->setRelativeFilePath(relativeFolderPathWithTrailingSlash + filenameOnly);
            }
            //else -- //no slash found is not an error, it just means that the path goes directly in the destination base dir -- so the filename by itself is already correct

            //We want to use the old timestamps from the original file, not the replacement file. We do want to use the fileSize and hash of the replacement file though
            generatedDestinationEasyTreeHashItem->setCreationDateTime(parsedSourceReplacementEasyTreeHashItem->creationDateTime());
            generatedDestinationEasyTreeHashItem->setLastModifiedDateTime(parsedSourceReplacementEasyTreeHashItem->lastModifiedDateTime());

            //no hash check since definitely using a new hash

            outputEasyTreeFileTextStream << generatedDestinationEasyTreeHashItem->toColonSeparatedLineOfText() << endl;
            ++numberOfEntriesProcessed;

            delete generatedDestinationEasyTreeHashItem;
        }
    }

    emit d("got to end of 'copyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFiles'... so if there weren't any errors then it probably worked");
    emit d(QString("number of entries processed: ") + QString::number(numberOfEntriesProcessed));
}
