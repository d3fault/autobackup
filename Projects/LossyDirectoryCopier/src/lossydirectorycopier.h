#ifndef LOSSYDIRECTORYCOPIER_H
#define LOSSYDIRECTORYCOPIER_H

#include <QCoreApplication>
#include <QObject>
#include <QDir>
#include <QProcess>
#include <QProcessEnvironment>
#include <QFileInfoList>
#include <QList>
#include <QListIterator>
#include <QStringList>
#include <QFile>

class LossyDirectoryCopier : public QObject
{
    Q_OBJECT
public:
    explicit LossyDirectoryCopier(QObject *parent = 0);

private:
    QList<QString> m_VideoConvertExtensions;
    QList<QString> m_ImageConvertExtensions;
    QList<QString> m_AudioConvertExtensions;
    bool recursivelyLossilyConvertOrCopyDirectoryEntries(QFileInfoList &rootFileInfoList);
    bool fileNameEndsWithOneOfTheseExentions(QString fileName, QList<QString> fileNameExtensions);

    QProcess m_FfmpegProcess;
    static const QString m_FFmpegProcessFilePath;
    QStringList m_FfMpegProcessVideoArgs;
    QStringList m_FfMpegProcessAudioArgs; //could just use opusenc, but ffmpeg accepts way more inputs...
    bool executeFfMpeg(QStringList ffmpegArguments);

    QString m_AbsoluteDestinationFolderPathWithSlashAppended;
    int m_AbsoluteSourceFolderPathWithSlashAppended_LENGTH;

    QProcess m_cWebPpPppPprocoessces;
    static const QString m_CwebPprocessFilePath;
    QStringList m_FfMpegProcessImageToPNGArgs;
    QStringList m_CwebPprocessArgs;
    void deleteFileIfExists(const QString &filePath);

    QDir::Filters m_DirFilter;

    bool m_StopRequested;
signals:
    void d(const QString &);
public slots:
    void lossilyCopyEveryDirectoryEntryAndJustCopyWheneverCantCompress(const QString &sourceDirString, const QString &destDirString);
    void requestStopAfterCurrentFileIsDone();
};

#endif // LOSSYDIRECTORYCOPIER_H
