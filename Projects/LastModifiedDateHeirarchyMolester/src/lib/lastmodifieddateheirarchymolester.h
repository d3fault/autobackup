#ifndef LASTMODIFIEDDATEHEIRARCHYMOLESTER_H
#define LASTMODIFIEDDATEHEIRARCHYMOLESTER_H

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QDir>
#include <QDateTime>
#include <QHash>
#include <QHashIterator>

#include "easytreehashitem.h"
#include "filemodificationdatechanger.h"

class LastModifiedDateHeirarchyMolester : public QObject
{
    Q_OBJECT
public:
    explicit LastModifiedDateHeirarchyMolester(QObject *parent = 0);
    bool loadFromXml(const QString &directoryHeirarchyCorrespingToXmlTreeFile, const QString &absoluteFilePathToXmlFormattedTreeFile);
    bool loadFromEasyTreeFile(const QString &directoryHeirarchyCorrespingToEasyTreeFile, const QString &absoluteFilePathToEasyTreeFile, bool easyTreeLinesHaveCreationDate);
    bool molestUsingInternalTables();
    bool loadAnyMissedFilesByRecursivelyScanningDirectoriesAndGiveThemThisTimestamp(const QString &absoluteDirToSearchForFilesIn, QDateTime dateTimeToGiveThem);

    //HACK:
    void performHackyOccuranceRateMerging(quint32 occuranceRateThresholdToTriggerIgnoringOfTimestamp);
    //</hack>
private:
    FileModificationDateChanger m_FileModificationDateChanger;
    QHash<QString,QDateTime> m_TimestampsByAbsoluteFilePathHash_Files;
    QHash<QString,QDateTime> m_TimestampsByAbsoluteFilePathHash_Folders;
    //HACK:
    bool m_FirstTimeDoingHackyOccuranceRateMerging;
    QHash<QString,QDateTime> m_OldTimestampsByAbsoluteFilePathHash_Files;
    QHash<QString,QDateTime> m_OldTimestampsByAbsoluteFilePathHash_Folders;
    //</hack>
    bool doSharedInitBetweenXmlAndEasyTree(const QString &directoryHeirarchyCorrespingToTimestampFile, const QString &timestampFilePath);

    bool recursivelyCheckForMissedFilesAndFolders(const QFileInfoList fileInfoList);

    QFile m_TimestampFile;
    QString m_DirectoryHeirarchyCorrespingToTimestampFileWithSlashAppended;
    QDateTime m_DateTimeToGiveMissedAndManuallyFoundFiles;
signals:
    void d(const QString &);
public slots:
    void molestLastModifiedDateHeirarchy(const QString &directoryHeirarchyCorrespingToEasyTreeFile, const QString &easyTreeFilePath, bool easyTreeLinesHaveCreationDate, bool xmlFormattedTreeCommandOutputActually = false);
};

#endif // LASTMODIFIEDDATEHEIRARCHYMOLESTER_H
