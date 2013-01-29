#include "threadhackqmake.h"

int ThreadHackQMake::runPreQmakeHack(const QString &buildDir, const QString &sourceDir)
{
    QString proFileIfExactlyOneFoundInSourceDir_OrEmptyStringIfNotFoundOrMoreThanOneFound = projectFilenameOnlyIfExactlyOneExists(sourceDir); //fuck this sucks because making having only one .pro file in the source dir a requirement of usage is just plain old stupid
    if(!proFileIfExactlyOneFoundInSourceDir_OrEmptyStringIfNotFoundOrMoreThanOneFound.isEmpty())
    {
        //fuck this sucks because I have to parse the .pro file and I can't just grab every .cpp/.h file because there tend to be custom as fuck rules on which files to include! I only want what SOURCES array and HEADERS array evaluates to at the end :-/
    }
    return -3;
}
int ThreadHackQMake::runPostQmakeHack(const QString &buildDir, const QString &sourceDir)
{
}
QString ThreadHackQMake::projectFilenameOnlyIfExactlyOneExists(const QString &dirToLookIn)
{
    QDir dir(dirToLookIn);
    if(dir.exists())
    {
        QStringList proFilter;
        proFilter << "*.pro";
        QStringList proFilesInDir = dir.entryList(proFilter);
        if(proFilesInDir.size() == 1)
        {
            if(QDir::exists(proFilesInDir.at(0)))
            {
                return proFilesInDir.at(0);
            }
        }
    }
    return QString("");
}
