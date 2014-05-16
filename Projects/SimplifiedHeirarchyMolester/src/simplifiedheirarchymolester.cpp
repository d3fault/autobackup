#include "simplifiedheirarchymolester.h"

SimplifiedHeirarchyMolester::SimplifiedHeirarchyMolester(QObject *parent) :
    QObject(parent)
{
}
void SimplifiedHeirarchyMolester::molestHeirarchy(const QString &dirToMolest, const QString &lastModifiedTimestampsFilePath)
{
    QString dirToMolestWithSlashAppended = dirToMolest;
    if(!dirToMolestWithSlashAppended.endsWith("/"))
    {
        dirToMolestWithSlashAppended.append("/");
    }
    QFileInfo dirToMolestFileInfo(dirToMolest);
    if(!dirToMolestFileInfo.exists())
    {
        emit d("Directory to Molest does Not Exist");
        return;
    }
    if(!dirToMolestFileInfo.isDir())
    {
        emit d("Directory to Molest is not a directory");
        return;
    }
    QFile lastModifiedTimestampsFile(lastModifiedTimestampsFilePath);
    if(!lastModifiedTimestampsFile.exists())
    {
        emit d("Timestamp File Does Not Exist");
        return;
    }
    if(!lastModifiedTimestampsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit d("Failed to open timestamp file for reading");
        return;
    }

    QTextStream lastModifiedTimestampsFileStream(&lastModifiedTimestampsFile);
    QString currentLine;
    FileModificationDateChanger fileModificationDateChanger;
    connect(&fileModificationDateChanger, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    emit d("Beginning molesting directory");
    while(!lastModifiedTimestampsFileStream.atEnd())
    {
        currentLine = lastModifiedTimestampsFileStream.readLine();
        if(!currentLine.isEmpty())
        {
            QScopedPointer<SimplifiedLastModifiedTimestamp> lastModifiedTimestampEntry(new SimplifiedLastModifiedTimestamp(currentLine));
            if(lastModifiedTimestampEntry->filePath().isEmpty())
            {
                emit d("found an erroneous entry in last modified timestamps file: " + currentLine);
                continue; //hmmm, or return? idfk
            }

            if(lastModifiedTimestampEntry->isDirectory())
            {
                QString dirPath = dirToMolestWithSlashAppended + lastModifiedTimestampEntry->filePath();
                if(!QFile::exists(dirPath))
                {
                    QDir dir(dirPath);
                    if(!dir.mkpath(dirPath))
                    {
                        emit d("while trying to make a dir in last modified timestamps file (that was empty so git missed it), we failed");
                        return;
                    }
                }
            }

            //TODOreq: unsure if it matters, but to be safe: molest all files and then all dirs

            if(!fileModificationDateChanger.changeModificationDate(dirToMolestWithSlashAppended + lastModifiedTimestampEntry->filePath(), lastModifiedTimestampEntry->lastModifiedTimestamp()))
            {
                emit d("failed to change modification date for an entry, so stopping");
                return;
            }
        }
    }
    emit d("Finished molesting directory");
}
