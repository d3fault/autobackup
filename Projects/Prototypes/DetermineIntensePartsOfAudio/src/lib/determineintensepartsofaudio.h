#ifndef DETERMINEINTENSEPARTSOFAUDIO_H
#define DETERMINEINTENSEPARTSOFAUDIO_H

#include <QObject>
#include <QScopedPointer>
#include <QTemporaryDir>
#include <QProcess>
#include <QRgb>
#include <QMultiMap>
#include <QList>

class DetermineIntensePartsOfAudio : public QObject
{
    Q_OBJECT
public:
    explicit DetermineIntensePartsOfAudio(QObject *parent = 0);
private:
    int m_MinDistanceMsBetweenIntenseParts;
    QScopedPointer<QTemporaryDir> m_TemporaryDir;
    QProcess *m_SoxProcess;
    QString m_SoxSpectrogramOutputPngFilePath;

    QMultiMap<QRgb, int> calculateAverageIntensitiesForAllFrequenciesAtAllPointsInTimeInSpectrogram(const QImage &soxSpectrogramImage);
    //int calculateNumIntenseParts(const QMultiMap<QRgb, int> &averageIntensitiesForAllFrequenciesAtAllPointsInTimeInSpectrogram, const int spectrogramWidth, const QRgb currentThreshold);
    QList<qint64> determineIntensePartsOfAudio(const QMultiMap<QRgb, int> &averageIntensitiesForAllFrequenciesAtAllPointsInTimeInSpectrogram, const QRgb threshold, const int spectrogramPixelsPerSecondOfAudio, const int targetNumIntensePartsInThisSong);
    qint64 determineMsTimestampForXpixelOnSpectrogram(const int xPixelOnSpectrogramToDetermineTimestampOf, const int spectrogramPixelsPerSecondOfAudio);
    bool tooCloseToAnotherIntensePart(qint64 timestampToCheck, const QList<qint64> &msTimestampsOfIntensePartsOfAudioToCheckAgainst);
signals:
    void e(const QString &msg);
    void doneDeterminingIntensePartsOfAudio(bool success, QList<qint64> msTimestampsOfMostIntensePartsOfAudio = QList<qint64>());
public slots:
    void startDeterminingIntensePartsOfAudio(const QString &audioFilePath, const int minDistanceMsBetweenIntenseParts = (1000/3));
private slots:
    void handleSoxProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // DETERMINEINTENSEPARTSOFAUDIO_H
