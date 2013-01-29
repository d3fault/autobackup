#ifndef THREADHACKQMAKE_H
#define THREADHACKQMAKE_H

#include <QString>
#include <QDir>

class ThreadHackQMake
{
public:
    static int runPreQmakeHack(const QString &buildDir, const QString &sourceDir);
    static int runPostQmakeHack(const QString &buildDir, const QString &sourceDir);

private:
    QString projectFilenameOnlyIfExactlyOneExists(const QString &dirToLookIn);
};

#endif // THREADHACKQMAKE_H
