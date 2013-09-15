#ifndef COPYANDMERGEANEASYTREEFILEANDAREPLACEMENTEASYTREEFILEWHILEVERIFYINGRELEVANTFILES_H
#define COPYANDMERGEANEASYTREEFILEANDAREPLACEMENTEASYTREEFILEWHILEVERIFYINGRELEVANTFILES_H

#include <QObject>
#include <QIODevice>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>

#include "easytreehashitem.h"
#include "easytreehasher.h"

class CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFiles : public QObject
{
    Q_OBJECT
public:
    explicit CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFiles(QObject *parent = 0);
private:
    inline QString appendTrailingSlashIfMissing(const QString theString) { if(theString.endsWith("/")){ return theString; } else { return (theString + QString("/")); } }
signals:
    void d(const QString &);
public slots:
    void copyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFiles(const QString &regularEasyTreeFileName, const QString &replacementFormattedEasyTreeFileName, const QString &sourceBaseDirCorrespondingToEasyTreeFile, const QString &destinationDirToCopyTo, const QString &outputEasyTreeFileName);
    void copyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFiles(QIODevice *regularEasyTreeFile, QIODevice *replacementFormattedEasyTreeFile, const QString &sourceBaseDirCorrespondingToEasyTreeFile, const QString &destinationDirToCopyTo, QIODevice *outputEasyTreeFile);
};

#endif // COPYANDMERGEANEASYTREEFILEANDAREPLACEMENTEASYTREEFILEWHILEVERIFYINGRELEVANTFILES_H
