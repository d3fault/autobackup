#ifndef RECURSIVEFILESTRINGREPLACERENAMER_H
#define RECURSIVEFILESTRINGREPLACERENAMER_H

#include <QObject>
#include <QDirIterator>
#include <QFileInfo>
#include <QList>

class RecursiveFileStringReplaceRenamer : public QObject
{
    Q_OBJECT
public:
    explicit RecursiveFileStringReplaceRenamer(QObject *parent = 0);

signals:
    void d(const QString &);
public slots:
    void recursivelyRenameFilesDoingSimpleStringReplace(const QString &rootDirectory, const QString &textToReplace, const QString &replacementText);
};

#endif // RECURSIVEFILESTRINGREPLACERENAMER_H
