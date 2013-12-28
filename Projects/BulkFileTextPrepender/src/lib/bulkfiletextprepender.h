#ifndef BULKFILETEXTPREPENDER_H
#define BULKFILETEXTPREPENDER_H

#include <QObject>
#include <QDirIterator>
#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QFileInfo>

class BulkFileTextPrepender : public QObject
{
    Q_OBJECT
public:
    explicit BulkFileTextPrepender(QObject *parent = 0);
private:
    bool filenameHasOneOfTheseExtensions(const QString &filename, const QStringList &extensions);
    bool prependTextToIoDevice(QIODevice *ioDeviceToPrependTextTo, const QString &textToPrepend);
signals:
    void d(const QString &);
public slots:
    void prependStringToBeginningOfAllTextFilesInDir(const QString &dir, const QString &filenameContainingTextToPrepend, const QStringList &filenameExtensionsToPrependTo);
    void prependStringToBeginningOfAllTextFilesInDir(const QString &dir, const QString &filenameContainingTextToPrepend, const QStringList &filenameExtensionsToPrependTo, QDir::Filters dirFilters, QDirIterator::IteratorFlags dirIteratorFlags);
};

#endif // BULKFILETEXTPREPENDER_H
