#ifndef DETERMINEINTENSEPARTSOFAUDIO_H
#define DETERMINEINTENSEPARTSOFAUDIO_H

#include <QObject>
#include <QScopedPointer>
#include <QTemporaryDir>
#include <QProcess>
#include <QRgb>
#include <QList>

class DetermineIntensePartsOfAudio : public QObject
{
    Q_OBJECT
public:
    explicit DetermineIntensePartsOfAudio(QObject *parent = 0);
private:
    int m_MaxIntensePartsPerSecond;
    QScopedPointer<QTemporaryDir> m_TemporaryDir;
    QProcess *m_SoxProcess;
    QString m_SoxSpectrogramOutputPngFilePath;

    int calculateNumIntenseParts(const QImage &soxSpectrogramImage, const QRgb currentThreshold);
    QList<quint64> determineIntensePartsOfAudio(const QImage &soxSpectrogramImage, const QRgb threshold, const int spectrogramPixelsPerSecondOfAudio);
    quint64 determineMsTimestampForXpixelOnSpectrogram(const int xPixelOnSpectrogramToDetermineTimestampOf, const int spectrogramPixelsPerSecondOfAudio);
signals:
    void e(const QString &msg);
    void doneDeterminingIntensePartsOfAudio(bool success, QList<quint64> msTimestampsOfMostIntensePartsOfAudio = QList<quint64>());
public slots:
    void startDeterminingIntensePartsOfAudio(const QString &audioFilePath, const int maxIntensePartsPerSecond = 3);
private slots:
    void handleSoxProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // DETERMINEINTENSEPARTSOFAUDIO_H
