#ifndef LASTMODIFIEDTIMESTAMPSTOOLS_H
#define LASTMODIFIEDTIMESTAMPSTOOLS_H

#include <QObject>

class LastModifiedTimestampsTools : public QObject
{
    Q_OBJECT
public:
    explicit LastModifiedTimestampsTools(QObject *parent = 0);
    bool generateLastModifiedTimestampsFile(const QString &dirToCreateLastModifiedTimestampsFor, const QString &lastModifiedTimestampsFilePath_WillBeOverwrittenIfExists);
    bool molestHeirarchy(const QString &dirToMolest, const QString &lastModifiedTimestampsFilePath);
private:
    inline QString appendSlashIfNeeded(const QString &inputString) { return inputString.endsWith("/") ? inputString : (inputString + "/"); }
signals:
    void d(const QString &);
    void lastModifiedTimestampsFileGenerated(bool);
    void heirarchyMolested(bool);
public slots:
    void beginGenerateLastModifiedTimestampsFile(const QString &dirToCreateLastModifiedTimestampsFor, const QString &lastModifiedTimestampsFilePath_WillBeOverwrittenIfExists);
    void beginMolestHeirarchy(const QString &dirToMolest, const QString &lastModifiedTimestampsFilePath);
};

#endif // LASTMODIFIEDTIMESTAMPSTOOLS_H
