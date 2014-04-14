#ifndef VIDEOSEGMENTSIMPORTERFOLDERWATCHER_H
#define VIDEOSEGMENTSIMPORTERFOLDERWATCHER_H

#include <QObject>
#include <QAtomicInt>

class QFileSystemWatcher;

class VideoSegmentsImporterFolderWatcher : public QObject
{
    Q_OBJECT
public:
    explicit VideoSegmentsImporterFolderWatcher(QAtomicInt *sharedVideoSegmentsArrayIndex, QObject *parent = 0);
private:
    QFileSystemWatcher *m_DirectoryWatcher;
    QAtomicInt *m_SharedVideoSegmentsArrayIndex;
public Q_SLOTS:
    void initializeAndStart();
private Q_SLOTS:
    void handleDirectoryChanged(const QString &path);
};

#endif // VIDEOSEGMENTSIMPORTERFOLDERWATCHER_H
