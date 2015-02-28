#ifndef FFMPEGSEGMENTUPLOADERCLI_H
#define FFMPEGSEGMENTUPLOADERCLI_H

#include <QObject>
#include <QTextStream>

#include "standardinputnotifier.h"
#include "../lib/ffmpegsegmentuploader.h"

class FfmpegSegmentUploaderCli : public QObject
{
    Q_OBJECT
public:
    explicit FfmpegSegmentUploaderCli(QObject *parent = 0);
private:
    StandardInputNotifier *m_StandardInputNotifier;
    FfmpegSegmentUploader *m_FfmpegSegmentUploader;
    QTextStream m_StdOut;
    QTextStream m_StdErr;

    void cliUsage();
    void cliUserInterfaceMenu();
signals:
    void tellSegmentInformationsRequested();
    void stopUploadingFfmpegSegmentsCleanlyRequested();
    void stopUploadingFfmpegSegmentsCleanlyUnlessDcRequested();
    void stopUploadingFfmpegSegmentsNowRequested();
private slots:
    void handleStandardInputReceived(const QString &standardInputLine);
    void handleO(const QString &msg);
    void handleE(const QString &msg);
    void handleAlertSegmentNotUploadedInCertainAmountOfTime(const QDateTime &dateTimeOfAlert);
};

#endif // FFMPEGSEGMENTUPLOADERCLI_H
