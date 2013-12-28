#include "recursivefilestringreplacerenamer.h"

RecursiveFileStringReplaceRenamer::RecursiveFileStringReplaceRenamer(QObject *parent) :
    QObject(parent)
{ }
void RecursiveFileStringReplaceRenamer::recursivelyRenameFilesDoingSimpleStringReplace(const QString &rootDirectory, const QString &textToReplace, const QString &replacementText)
{
    QList<QFileInfo> listOfAbsolutePathsNeedingReplacement; //dir iterator isn't mutable (and would be confusing if it was (unless properly documented))

    QDirIterator dirIterator(rootDirectory, (QDir::Dirs | QDir::Files | QDir::NoSymLinks |QDir::NoDotAndDotDot | QDir::Hidden), QDirIterator::Subdirectories);
    while(dirIterator.hasNext())
    {
        dirIterator.next();
        const QFileInfo &currentFileInfo = dirIterator.fileInfo();
        if(!currentFileInfo.isFile())
            continue;
        if(!dirIterator.fileName().contains(textToReplace))
            continue;
        listOfAbsolutePathsNeedingReplacement.append(currentFileInfo);
    }
    //now iterate the list we built, doing the actual renaming
    foreach(QFileInfo currentFileInfo, listOfAbsolutePathsNeedingReplacement)
    {
        QString currentFilePathFolderWithSlashAppended = currentFileInfo.absolutePath();
        if(!currentFilePathFolderWithSlashAppended.endsWith("/"))
            currentFilePathFolderWithSlashAppended.append("/");
        QString filenameOnly = currentFileInfo.fileName();
        QString filenameReplaced = filenameOnly.replace(textToReplace, replacementText);
        if(!QFile::rename(currentFileInfo.absoluteFilePath(), currentFilePathFolderWithSlashAppended + filenameReplaced))
        {
            emit d("failed to rename file:" + currentFileInfo.absoluteFilePath());
            return;
        }
        emit d("renamed file to: " + filenameReplaced);
    }
    emit d("done renaming files");
}
