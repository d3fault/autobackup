#include "lastmodifiedtimestampstools.h"

#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QScopedPointer>
#include <QDirIterator>

#include "lastmodifiedtimestamp.h"
#include "filemodificationdatechanger.h"

//TODOreq: implement the filename/dir/etc filters that QuickDirty has... to skip binaries etc. also maybe sort the entries the same was as quickdirty.. but idfk

LastModifiedTimestampsTools::LastModifiedTimestampsTools(QObject *parent)
    : QObject(parent)
{ }
bool LastModifiedTimestampsTools::generateLastModifiedTimestampsFile(const QString &dirToCreateLastModifiedTimestampsFor, const QString &lastModifiedTimestampsFilePath_WillBeOverwrittenIfExists)
{
    QString dirToCreateLastModifiedTimestampsForWithSlashAppended = appendSlashIfNeeded(dirToCreateLastModifiedTimestampsFor);
    QFileInfo dirToCreateLastModifiedTimestampsForFileInfo(dirToCreateLastModifiedTimestampsForWithSlashAppended);
    if(!dirToCreateLastModifiedTimestampsForFileInfo.exists())
    {
        emit d("Directory to generate last modified timestamps for does not exist");
        return false;
    }
    if(!dirToCreateLastModifiedTimestampsForFileInfo.isDir())
    {
        emit d("Directory to generate last modified timestamps for is not a directory");
        return false;
    }
    QFileInfo lastModifiedTimestampsFilePathFileInfo(lastModifiedTimestampsFilePath_WillBeOverwrittenIfExists);
    const QString &absoluteLastModifiedTimestampsFilePath = lastModifiedTimestampsFilePathFileInfo.absoluteFilePath(); //can't qfiledialog give me relative? methinks yes. only matters really for the not recording timestamps file's timestamp hack anyways
    QFile lastModifiedTimestampsFile(absoluteLastModifiedTimestampsFilePath);
    if(!lastModifiedTimestampsFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        emit d("Failed to open timestamp file for writing");
        return false;
    }
    QTextStream lastModifiedTimestampsFileStream(&lastModifiedTimestampsFile);
    QDirIterator dirIterator(dirToCreateLastModifiedTimestampsFor, (QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Hidden), QDirIterator::Subdirectories);
    int dirToCreateLastModifiedTimestampsForWithSlashAppendedLength = dirToCreateLastModifiedTimestampsForWithSlashAppended.length();
    emit d("Beginning generating last modified timestamps for directory");
    qint64 entryCount = 0;
    while(dirIterator.hasNext())
    {
        dirIterator.next();
        const QFileInfo &currentEntryFileInfo = dirIterator.fileInfo();
        if(currentEntryFileInfo.isFile() || currentEntryFileInfo.isDir())
        {
            const QString &currentEntryAbsoluteFilePath = currentEntryFileInfo.absoluteFilePath();
            if(currentEntryAbsoluteFilePath == absoluteLastModifiedTimestampsFilePath)
                continue; //hack: don't record timestamp of timestamps file (we are writing to it as we speak!)
            ++entryCount;
            QString relativePathNoLeadingSlash = currentEntryAbsoluteFilePath.mid(dirToCreateLastModifiedTimestampsForWithSlashAppendedLength);
            SimplifiedLastModifiedTimestamp currentEntryLastModifiedTimestamp(relativePathNoLeadingSlash, currentEntryFileInfo.lastModified(), (currentEntryFileInfo.isFile() ? SimplifiedLastModifiedTimestamp::IsNotDirectory : SimplifiedLastModifiedTimestamp::IsDirectory));
            lastModifiedTimestampsFileStream << currentEntryLastModifiedTimestamp.toColonSeparatedLineOfText() << endl; //eh could implement text stream operators, but who says it should always be colon separated (i guess i do/can)?
        }
    }
    emit d("Finished generating last modified timestamps for directory. Number of entries: " + QString::number(entryCount));
    return true;
}
bool LastModifiedTimestampsTools::molestHeirarchy(const QString &dirToMolest, const QString &lastModifiedTimestampsFilePath)
{
    QString dirToMolestWithSlashAppended = dirToMolest;
    if(!dirToMolestWithSlashAppended.endsWith("/"))
    {
        dirToMolestWithSlashAppended.append("/");
    }
    QFileInfo dirToMolestFileInfo(dirToMolest);
    if(!dirToMolestFileInfo.exists())
    {
        emit d("Directory to molest does not exist");
        return false;
    }
    if(!dirToMolestFileInfo.isDir())
    {
        emit d("Directory to molest is not a directory");
        return false;
    }
    QFile lastModifiedTimestampsFile(lastModifiedTimestampsFilePath);
    if(!lastModifiedTimestampsFile.exists())
    {
        emit d("Timestamp file does not exist");
        return false;
    }
    if(!lastModifiedTimestampsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit d("Failed to open timestamp file for reading");
        return false;
    }

    QTextStream lastModifiedTimestampsFileStream(&lastModifiedTimestampsFile);
    QString currentLine;
    FileModificationDateChanger fileModificationDateChanger;
    connect(&fileModificationDateChanger, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    emit d("Beginning molesting directory");
    qint64 entryCount = 0;
    while(!lastModifiedTimestampsFileStream.atEnd())
    {
        currentLine = lastModifiedTimestampsFileStream.readLine();
        if(!currentLine.isEmpty())
        {
            QScopedPointer<SimplifiedLastModifiedTimestamp> lastModifiedTimestampEntry(new SimplifiedLastModifiedTimestamp(currentLine));
            if(lastModifiedTimestampEntry->filePath().isEmpty())
            {
                emit d("Found an erroneous entry in last modified timestamps file: " + currentLine);
                continue; //hmmm, or return? idfk
            }

            ++entryCount;

            if(lastModifiedTimestampEntry->isDirectory())
            {
                QString dirPath = dirToMolestWithSlashAppended + lastModifiedTimestampEntry->filePath();
                if(!QFile::exists(dirPath))
                {
                    QDir dir(dirPath);
                    if(!dir.mkpath(dirPath))
                    {
                        emit d("While trying to make a dir in last modified timestamps file (that was empty so git missed it), we failed");
                        return false;
                    }
                }
            }

            //TODOreq: unsure if it matters, but to be safe: molest all files and then all dirs

            QString filePathToMolest = dirToMolestWithSlashAppended + lastModifiedTimestampEntry->filePath();
            if(!QFile::exists(filePathToMolest))
            {
                emit d("File path does not exist: " + filePathToMolest);
                //return false; //we could just continue;
                continue; //or maybe we should return false (proper solution = cli arg (or gui checkbox) to choose what to do)
            }
            if(!fileModificationDateChanger.changeModificationDate(filePathToMolest, lastModifiedTimestampEntry->lastModifiedTimestamp()))
            {
                emit d("Failed to change modification date for an entry, so stopping");
                return false;
            }
        }
    }
    emit d("Finished molesting directory. Number of entries: " + QString::number(entryCount));
    return true;
}
void LastModifiedTimestampsTools::beginGenerateLastModifiedTimestampsFile(const QString &dirToCreateLastModifiedTimestampsFor, const QString &lastModifiedTimestampsFilePath_WillBeOverwrittenIfExists)
{
    emit lastModifiedTimestampsFileGenerated(generateLastModifiedTimestampsFile(dirToCreateLastModifiedTimestampsFor, lastModifiedTimestampsFilePath_WillBeOverwrittenIfExists));
}
void LastModifiedTimestampsTools::beginMolestHeirarchy(const QString &dirToMolest, const QString &lastModifiedTimestampsFilePath)
{
    emit heirarchyMolested(molestHeirarchy(dirToMolest, lastModifiedTimestampsFilePath));
}
