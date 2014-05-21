#include "bulkfiletextprepender.h"

//#define INSERT_ON_SECOND_LINE_HACK 1

BulkFileTextPrepender::BulkFileTextPrepender(QObject *parent) :
    QObject(parent)
{
}
//"what" should already be lowercase, and so should all the extensions
bool BulkFileTextPrepender::filenameHasOneOfTheseExtensions(const QString &filename, const QStringList &extensions)
{
    foreach(const QString &extension, extensions)
    {
        if(filename.endsWith("." + extension))
        {
            return true;
        }
    }
    return false;
}
//assumes the file will fit in RAM (although I guess this is why swap was invented (STILL, swap might not be enough hurr!) xD)
//pass the iodevice in closed, and it will finish closed as well
bool BulkFileTextPrepender::prependTextToIoDevice(QIODevice *ioDeviceToPrependTextTo, const QString &textToPrepend)
{
    if(ioDeviceToPrependTextTo->isSequential())
    {
        emit d("can't prepend to sequential device");
        return false;
    }
    if(!ioDeviceToPrependTextTo->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit d("ERROR: failed to open file for reading");
        return false;
    }
    QTextStream textStream(ioDeviceToPrependTextTo);
#ifdef INSERT_ON_SECOND_LINE_HACK //bash and svg/xml files -- ASSUMES > 1 line
    QString firstLine = textStream.readLine();
    textStream.seek(0);
    QString allFileData = textStream.readAll();
    QString allFileDataExceptFirstLine = allFileData.right(allFileData.length() - firstLine.length());

    QString newFileData = firstLine + "\n" + textToPrepend + allFileDataExceptFirstLine;
#else
    QString newFileData = textToPrepend + textStream.readAll();
#endif
    if(!textStream.seek(0))
    {
        emit d("failed to seek to beginning of file");
        return false;
    }
    ioDeviceToPrependTextTo->close();
    if(!ioDeviceToPrependTextTo->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        emit d("ERROR: failed to open file for writing");
        return false;
    }
    textStream << newFileData;
    ioDeviceToPrependTextTo->close();
    return true;
}
//filenameExtensionsToPrependTo, empty signifies all extensions
void BulkFileTextPrepender::prependStringToBeginningOfAllTextFilesInDir(const QString &dir, const QString &filenameContainingTextToPrepend, const QStringList &filenameExtensionsToPrependTo)
{
    prependStringToBeginningOfAllTextFilesInDir(dir, filenameContainingTextToPrepend, filenameExtensionsToPrependTo, (QDir::Dirs | QDir::Files | QDir::NoSymLinks |QDir::NoDotAndDotDot | QDir::Hidden), QDirIterator::Subdirectories /*recursive by default*/);
}
void BulkFileTextPrepender::prependStringToBeginningOfAllTextFilesInDir(const QString &dir, const QString &filenameContainingTextToPrepend, const QStringList &filenameExtensionsToPrependTo, QDir::Filters dirFilters, QDirIterator::IteratorFlags dirIteratorFlags)
{

    QFile textToPrependFile(filenameContainingTextToPrepend);
    if(!textToPrependFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit d("failed to open file with text to prepend: " + filenameContainingTextToPrepend);
        return;
    }
    QTextStream blah(&textToPrependFile);
    QString textToPrepend = blah.readAll();
    if(textToPrepend.isEmpty())
    {
        emit d("eh why you prepending an empty file?");
        return;
    }
    textToPrependFile.close();

    QDirIterator dirIterator(dir, dirFilters, dirIteratorFlags);
    while(dirIterator.hasNext())
    {
        dirIterator.next();
        const QFileInfo &currentFileInfo = dirIterator.fileInfo();
        if(currentFileInfo.isFile())
        {
            if(currentFileInfo.absoluteFilePath() == filenameContainingTextToPrepend) //don't prepend onto the prepend source!
                continue;

            QString filename = dirIterator.fileName();
            if(filenameExtensionsToPrependTo.isEmpty() || filenameHasOneOfTheseExtensions(filename.toLower(), filenameExtensionsToPrependTo))
            {
                QFile theFile(dirIterator.filePath());
                if(!prependTextToIoDevice(&theFile, textToPrepend))
                {
                    emit d("ERROR: failed to prepend text to file: " + dirIterator.filePath());
                    return;
                }
                emit d("prepended text to: " + filename);
            }
        }
    }
    emit d("done prepending");
}
