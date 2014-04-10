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

    void cliUsage();
    void cliUserInterfaceMenu();
signals:
    void tellSegmentInformationsRequested();
    void stopUploadingFfmpegSegmentsRequested();
private slots:
    void handleStandardInputReceived(const QString &standardInputLine);
    void handleD(const QString &msg);
};

#endif // FFMPEGSEGMENTUPLOADERCLI_H
