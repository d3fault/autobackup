#ifndef EBOOKREADERBOOKMARKSFILEURLEXTRACTOR_H
#define EBOOKREADERBOOKMARKSFILEURLEXTRACTOR_H

#include <QObject>

class QIODevice;

class eBookReaderBookmarksFileUrlExtractor : public QObject
{
    Q_OBJECT
public:
    explicit eBookReaderBookmarksFileUrlExtractor(QObject *parent = 0);
signals:
    void urlExtracted(const QString &url); //QUrl? naww dawg naww
    void finishedExtractingUrlsFromEbookReaderBookmarksFile(bool success);
public slots:
    void startExtractingUrlsFromEbookReaderBookmarksFile(QIODevice *bookmarksFileIoDevice);
    void startExtractingUrlsFromEbookReaderBookmarksFile(const QString &bookmarksFilePath);
};

#endif // EBOOKREADERBOOKMARKSFILEURLEXTRACTOR_H
