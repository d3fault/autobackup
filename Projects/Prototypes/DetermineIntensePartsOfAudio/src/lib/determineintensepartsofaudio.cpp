#include "determineintensepartsofaudio.h"

#include <QImage>
#include <QColor>

#define DetermineIntensePartsOfAudio_spectrogramPixelsPerSecondOfAudio 300 //TODOreq: sent to spectrogram process call, and also used when analyzing the resulting png (maybe this should be an arg)
#define DetermineIntensePartsOfAudio_songDurationMilliseconds 5000 //TODOreq: ffprobe -- or actually we could even calculate it by using the width of the spectrogram and performing a calculation with spectrogramPixelsPerSecondOfAudio (one less dependency is one less dependency)

//Audio file -> Sox spectrogram -> custom png analyzer to determine timestamps of most 'intense' parts
DetermineIntensePartsOfAudio::DetermineIntensePartsOfAudio(QObject *parent)
    : QObject(parent)
    , m_SoxProcess(new QProcess(this))
{
    m_SoxProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_SoxProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(handleSoxProcessFinished(int,QProcess::ExitStatus)));
}
int DetermineIntensePartsOfAudio::calculateNumIntenseParts(const QImage &soxSpectrogramImage, const QRgb currentThreshold)
{
    //TODOprobably: have a minimum amount of x pixels (time) that is considered a single "part", instead of just a single pixel. aka group together somehow (average together? idfk) -- some consideration needs to be given into the fact that there might be one continuous/long "intense" part (think:a trumpet being blown for 5 seconds). technically all 5 seconds of that trumpet SHOULD be one 'part'. would we want to report the start timestamp of that intense part, the end, or the start + duration? DETERMINING _DYNAMICALLY_ length'd 'parts' sounds like a bitch (but is surely possible ('edge' detection algorithms (in images, but oh look at that our audio is represented as an image :-D) already exist, it's just a matter of figuring out how to use one)) -- but maybe initially and to KISS, i should just use a simplier (lol typo, keeping it) 'parts' model of some N number of pixels wide

    int numIntenseParts = 0;
    int imageWidth = soxSpectrogramImage.width();
    int imageHeight = soxSpectrogramImage.height();
    for(int x = 0; x < imageWidth; ++x)
    {
        //calculate average intensity for all frequencies (y) at that point in time (x). TODOoptimization: this isn't going to change in between each call to calculateNumIntenseParts
        QRgb averageIntensityForAllFrequenciesAtThatPointInTime = 0;
        for(int y = 0; y < imageHeight; ++y)
        {
            averageIntensityForAllFrequenciesAtThatPointInTime += soxSpectrogramImage.pixel(x, y);
        }
        averageIntensityForAllFrequenciesAtThatPointInTime /= imageHeight;

        //does that averaged intensity meet the threshold? if so, increase the number of intense parts
        if(averageIntensityForAllFrequenciesAtThatPointInTime > currentThreshold)
        {
            ++numIntenseParts;
        }
    }
    return numIntenseParts;
}
QList<quint64> DetermineIntensePartsOfAudio::determineIntensePartsOfAudio(const QImage &soxSpectrogramImage, const QRgb threshold, const int spectrogramPixelsPerSecondOfAudio)
{
    //TODOoptional: merge this method with calculateNumIntenseParts, as they are very similar

    QList<quint64> msTimestampsOfIntensePartsOfAudio;
    int imageWidth = soxSpectrogramImage.width();
    int imageHeight = soxSpectrogramImage.height();
    for(int x = 0; x < imageWidth; ++x)
    {
        //calculate average intensity for all frequencies (y) at that point in time (x). TODOoptimization: this has already been calculated in calculateNumIntenseParts
        QRgb averageIntensityForAllFrequenciesAtThatPointInTime = 0;
        for(int y = 0; y < imageHeight; ++y)
        {
            averageIntensityForAllFrequenciesAtThatPointInTime += soxSpectrogramImage.pixel(x, y);
        }
        averageIntensityForAllFrequenciesAtThatPointInTime /= imageHeight;

        //does that averaged intensity meet the threshold? if so, append it. TODOreq: factor in m_MaxIntensePartsPerSecond (related TODOmb: should I factor in m_MaxIntensePartsPerSecond in calculateNumIntenseParts as well? (it would implicitly decrease the num intense parts ofc))
        if(averageIntensityForAllFrequenciesAtThatPointInTime > threshold)
        {
            msTimestampsOfIntensePartsOfAudio.append(determineMsTimestampForXpixelOnSpectrogram(x, spectrogramPixelsPerSecondOfAudio));
        }
    }
    return msTimestampsOfIntensePartsOfAudio;
}
quint64 DetermineIntensePartsOfAudio::determineMsTimestampForXpixelOnSpectrogram(const int xPixelOnSpectrogramToDetermineTimestampOf, const int spectrogramPixelsPerSecondOfAudio)
{
    //pixel 1 of a spectrogram with 100 pixels per second would have a timestamp of 10ms
    //(1000 * 1) / 100 = 10ms
    return (1000 * xPixelOnSpectrogramToDetermineTimestampOf) / spectrogramPixelsPerSecondOfAudio; //learned cross multiplication in the third grade
}
void DetermineIntensePartsOfAudio::startDeterminingIntensePartsOfAudio(const QString &audioFilePath, const int maxIntensePartsPerSecond)
{
    m_MaxIntensePartsPerSecond = maxIntensePartsPerSecond;
    if(m_TemporaryDir.isNull())
    {
        m_TemporaryDir.reset(new QTemporaryDir());
        if(!m_TemporaryDir->isValid())
        {
            emit e("failed to get temporary dir: " + m_TemporaryDir->path());
            emit doneDeterminingIntensePartsOfAudio(false);
            return;
        }
        m_SoxSpectrogramOutputPngFilePath = m_TemporaryDir->path() + QDir::separator() + "spectrogram.png"; //TODOoptional: ideally libsox would be used to stay off of the fs entirely
    }
    //sox ark.wav -n remix 1-2 rate 3k spectrogram -r -h -w Rectangular -X 250 -s -o output.png
    QStringList soxSpectrogramArgs;
    soxSpectrogramArgs << audioFilePath << "-n" << "remix" << "1-2" /* TODOoptional: determine num channels via ffprobe, or maybe some other way to mix all channels down to one channel*/ << "rate" << "3k" << "spectrogram" << "-r" << "-h" << "-w" << "Rectangular" << "-X" << QString::number(DetermineIntensePartsOfAudio_spectrogramPixelsPerSecondOfAudio) << "-s" << "-o" << m_SoxSpectrogramOutputPngFilePath;
    m_SoxProcess->start("sox", soxSpectrogramArgs, QIODevice::ReadOnly);
    if(!m_SoxProcess->waitForStarted(-1)) //either this or I have to listen for QProcess::error ... this is easier (and I still don't know if finished can ever be emitted when an error does also... but i think yes! (however i also think that if it starts, then the finished signal will always be emitted))
    {
        emit e("sox failed to start");
        emit doneDeterminingIntensePartsOfAudio(false);
        return;
    }
}
void DetermineIntensePartsOfAudio::handleSoxProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QString soxStdOutAndStdErr = m_SoxProcess->readAll();
    if(exitCode != 0 || exitStatus != QProcess::NormalExit)
    {
        emit e(soxStdOutAndStdErr);
        emit e("sox exitted abnormally with exit code: " + QString::number(exitCode));
        emit doneDeterminingIntensePartsOfAudio(false);
        return;
    }

    QImage soxSpectrogramImage(m_SoxSpectrogramOutputPngFilePath, "png");

    //I don't want to have a guaranteed "one intense point per second (the most intense point per second)", so I suppose I need a minimum threshold. I need to determine that minimum threshold on a per-song (or maybe per-segment TODOreq) basis (aka dynamically, not hardcoded), and to do that I need a "target intense points per song (or per minute/etc)"... which I can calculate using maxIntensePartsPerSecond... but I think I want to increase maxIntensePartsPerSecond (double it?) before using it for that particular calculation... because I want to have "more intense points than necessary" in order to allow me to filter down into using maxIntensePartsPerSecond for it's traditional/standard usage (the one it's variable name describes)
    //After some more thinking about it, maybe actually I want to use HALF my maxIntensePartsPerSecond instead of DOUBLE it. reason: we don't want to desperately search for 'intense' points when there is a chunk of silence (not meeting maxIntensePartsPerSecond for a given second is perfectly fine)

    //Determine song-wide threshold for what is considered an intense part
    const int songDurationSeconds = (DetermineIntensePartsOfAudio_songDurationMilliseconds / 1000);
    //const int targetIntensePartsInThisSong = (m_MaxIntensePartsPerSecond * songDurationSeconds) * 2;
    const int targetIntensePartsInThisSong = (m_MaxIntensePartsPerSecond * songDurationSeconds) / 2; //TODOreq: try uncommenting above 'times two' formula, i'm not sure which I want
    //start with everything qualifying as intense part and then remove until hit target, or vice versa? I suppose it doesn't matter
    QRgb currentThreshold = QColor(255, 255, 255).rgb(); //start with (most probably) nothing qualifying as an intense part, then decrease the threshold until target num intense parts is met. TODOreq: for the default mode of operation, yes it is a good idea to dynamically calculate the threshold, but the user should be able to specify the threshold as an arg as well (overriding the dynamic calculation). the user specifying it would probably (after some trial and error) give much better results
    while(calculateNumIntenseParts(soxSpectrogramImage, currentThreshold) < targetIntensePartsInThisSong)
    {
        --currentThreshold; //TODOoptimization: maybe -= 5 (etc) to speed things up?
    }
    //at this point, currentThreshold points to a threshold that makes us slightly exceed (because hitting it exactly would be unlikely) targetIntensePartsInThisSong

    //Now determine the most intense parts of the audio, using that threshold, and also making sure not to exceed m_MaxIntensePartsPerSecond
    QList<quint64> msTimestampsOfIntensePartsOfAudio = determineIntensePartsOfAudio(soxSpectrogramImage, currentThreshold, DetermineIntensePartsOfAudio_spectrogramPixelsPerSecondOfAudio);
    emit doneDeterminingIntensePartsOfAudio(true, msTimestampsOfIntensePartsOfAudio);
}
