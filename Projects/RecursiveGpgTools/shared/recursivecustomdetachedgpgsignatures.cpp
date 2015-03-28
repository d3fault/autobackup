#include "recursivecustomdetachedgpgsignatures.h"

#include <QDateTime>
#include <QTextStream>

#define GPG_END_SIG_DELIMITER "-----END PGP SIGNATURE-----"

#define RecursiveCustomDetachedSignatures_DELIMETER ":"
#define RecursiveCustomDetachedSignatures_ESCAPED_DELIMETER "\\" RecursiveCustomDetachedSignatures_DELIMETER
#define RecursiveCustomDetachedSignatures_ILLEGAL_FILENAME_NOT_CONTAINING_COLON "kjxx..../////.\\\\^^^\n\taaaa1   23333358 324434 sdfsfd sdfsfsfsf sersiously there are no fucking blah characters anymore that are illegal and idfk how to type a null character but i guess the longer i make this, the less likely a collision is to happen 0 4398489''';;\\kthx"

RecursiveCustomDetachedSignatures::RecursiveCustomDetachedSignatures(QObject *parent)
    : QObject(parent)
{ }
bool RecursiveCustomDetachedSignatures::readInAllSigsFromSigFile(QIODevice *sigsFile, QHash<QString /*file path*/, RecursiveCustomDetachedSignaturesFileMeta /*file meta*/> *sigsFromSigFile)
{
    QTextStream sigsFileTextStream(sigsFile);
    while(!sigsFileTextStream.atEnd())
    {
        QString alreadySignedFilePath;
        QString alreadySignedFileSig;
        qint64 alreadySignedFileUnixTimestamp;
        if(!readPathAndSignature(sigsFileTextStream, &alreadySignedFilePath, &alreadySignedFileSig, &alreadySignedFileUnixTimestamp))
            return false;
        sigsFromSigFile->insert(alreadySignedFilePath, RecursiveCustomDetachedSignaturesFileMeta(alreadySignedFilePath, alreadySignedFileSig, alreadySignedFileUnixTimestamp));
    }
    return true;
}
bool RecursiveCustomDetachedSignatures::readPathAndSignature(QTextStream &customDetachedGpgSignaturesTextStream, QString *out_FilePathToVerify, QString *out_CurrentFileSignature, qint64 *out_CurrentFileUnixTimestamp)
{
    const QString &origFilePathAndTimestampLine = customDetachedGpgSignaturesTextStream.readLine();
    QString filePathAndUnixTimestamp = origFilePathAndTimestampLine;
    filePathAndUnixTimestamp.replace(RecursiveCustomDetachedSignatures_ESCAPED_DELIMETER, RecursiveCustomDetachedSignatures_ILLEGAL_FILENAME_NOT_CONTAINING_COLON);
    QStringList filePathAndUnixTimestamp_Split = filePathAndUnixTimestamp.split(RecursiveCustomDetachedSignatures_DELIMETER);
    if(filePathAndUnixTimestamp_Split.size() != 2)
    {
        emit e("malformed file path and unix timestamp line: " + origFilePathAndTimestampLine);
        return false;
    }
    QString filePath = filePathAndUnixTimestamp_Split.first();
    filePath.replace(RecursiveCustomDetachedSignatures_ILLEGAL_FILENAME_NOT_CONTAINING_COLON, RecursiveCustomDetachedSignatures_DELIMETER);
    *out_FilePathToVerify = filePath;
    bool convertOk = false;
    qint64 unixTimestamp = filePathAndUnixTimestamp_Split.last().toLongLong(&convertOk);
    if(!convertOk || unixTimestamp < 0)
    {
        emit e("malformed unix timestamp on line: " + origFilePathAndTimestampLine);
        return false;
    }
    *out_CurrentFileUnixTimestamp = unixTimestamp;
    QString lastReadLine;
    do
    {
        if(customDetachedGpgSignaturesTextStream.atEnd())
        {
            emit e("malformed custom detached gpg signatures file near file path: " + *out_FilePathToVerify);
            return false;
        }
        lastReadLine = customDetachedGpgSignaturesTextStream.readLine(); //TO DOnereq(/dumb): getting a consistent segfault (sigsegv) right here and have no clue why, I'm wondering if it's a Qt bug. different dirs/sig files crash at the same spot every time. tried all sorts of combinations/rewrites. rolled back to before latest addition, still there. shit rolled back to first commits of this app, still there. WTF!?!? --- maybe 30 minutes later: omg figured it out, my QFile was going out of scope (deep down I knew it was my fault). How the fuck did I see like 10+ files have their sigs verified then? That's even weirder, though I don't care to try to understand why...
        out_CurrentFileSignature->append(lastReadLine + "\n"); //TODOoptional: might be better to not use readLine up above and to just read until the string is seen (retain whatever newline form the output of gpg gave us to begin with)
    }
    while(lastReadLine != GPG_END_SIG_DELIMITER);
    return true;
}
bool RecursiveCustomDetachedSignatures::filesystemLastModifiedUnixTimestampAndMetaUnixTimestampsAreIdentical(qint64 filesystemLastModifiedUnixTimestampInSeconds, const RecursiveCustomDetachedSignaturesFileMeta &fileMeta)
{
    if(fileMeta.UnixTimestampInSeconds != filesystemLastModifiedUnixTimestampInSeconds)
    {
        emit e("last modified timestamp on filesystem differs from what's in the sigs file: " + fileMeta.FilePath);
        emit e("on fs: " + QString::number(filesystemLastModifiedUnixTimestampInSeconds) + " -- in sigs file: " + QString::number(fileMeta.UnixTimestampInSeconds));
        return false;
    }
    return true;
}
QTextStream &operator<<(QTextStream &textStream, const RecursiveCustomDetachedSignaturesFileMeta &fileMeta) //TODOoptional: stream in operator (except streaming in can fail... so maybe NOT)
{
    QString filePath = fileMeta.FilePath;
    filePath.replace(RecursiveCustomDetachedSignatures_DELIMETER, RecursiveCustomDetachedSignatures_ESCAPED_DELIMETER);
    textStream << filePath << RecursiveCustomDetachedSignatures_DELIMETER << fileMeta.UnixTimestampInSeconds << endl << fileMeta.GpgSignature;
    return textStream;
}
bool RecursiveCustomDetachedSignaturesFileMeta::operator==(const RecursiveCustomDetachedSignaturesFileMeta &other) const
{
    return ((other.FilePath == FilePath) && (other.GpgSignature == GpgSignature) && (other.UnixTimestampInSeconds == UnixTimestampInSeconds));
}
