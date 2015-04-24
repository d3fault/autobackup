#include "determineintensepartsofaudio.h"

#include <QImage>
#include <QColor>
#include <QMapIterator>

#define DetermineIntensePartsOfAudio_spectrogramPixelsPerSecondOfAudio 300 //sent to spectrogram process call, and also used when analyzing the resulting png (maybe this should be an arg)
#define DetermineIntensePartsOfAudio_songDurationMilliseconds 5000 //TODOreq: ffprobe -- or actually we could even calculate it by using the width of the spectrogram and performing a calculation with spectrogramPixelsPerSecondOfAudio (one less dependency is one less dependency)


#define DetermineIntensePartsOfAudio_SOX_Q_ARG "3" //if changing this, use a color picker on the resulting images to change the below define along with it (aka: don't change)
#define DetermineIntensePartsOfAudio_SOX_Q_ARG_RESULTS_IN_COLOR_TO_FILTER_OUT "#4000ff" //TODOreq: cross platform? does sox on every platform use this color whenever -q (defined directly above) is 3?

//Audio file -> Sox spectrogram -> custom png analyzer to determine timestamps of most 'intense' parts
DetermineIntensePartsOfAudio::DetermineIntensePartsOfAudio(QObject *parent)
    : QObject(parent)
    , m_SoxProcess(new QProcess(this))
{
    m_SoxProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_SoxProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(handleSoxProcessFinished(int,QProcess::ExitStatus)));
}
QMultiMap<QRgb, int> DetermineIntensePartsOfAudio::calculateAverageIntensitiesForAllFrequenciesAtAllPointsInTimeInSpectrogram(const QImage &soxSpectrogramImage)
{
    QMultiMap<QRgb, int> averageIntensitiesForAllFrequenciesAtAllPointsInTimeInSpectrogram;
    int imageWidth = soxSpectrogramImage.width();
    int imageHeight = soxSpectrogramImage.height();
    QRgb filterOutColor = QColor(DetermineIntensePartsOfAudio_SOX_Q_ARG_RESULTS_IN_COLOR_TO_FILTER_OUT).rgb();
    for(int x = 0; x < imageWidth; ++x)
    {
        qint64 averageIntensityForAllFrequenciesAtThatPointInTime = 0;
        int heightToUseForAveraging = 0;
        for(int y = 0; y < imageHeight; ++y)
        {

            QRgb colorAtCurrentPixel = soxSpectrogramImage.pixel(x, y);
            if(colorAtCurrentPixel != filterOutColor) //filter out 4000ff (which changes based on -q). there's also 'black' i _could_ filter out, but it's basically a noop to do so (since black == zero (racist))
            {
                averageIntensityForAllFrequenciesAtThatPointInTime += colorAtCurrentPixel;
                ++heightToUseForAveraging;
            }
        }
        if(heightToUseForAveraging > 0) //don't divide by zero xD
            averageIntensityForAllFrequenciesAtThatPointInTime /= heightToUseForAveraging;
        averageIntensitiesForAllFrequenciesAtAllPointsInTimeInSpectrogram.insert(static_cast<QRgb>(averageIntensityForAllFrequenciesAtThatPointInTime), x);
    }
    return averageIntensitiesForAllFrequenciesAtAllPointsInTimeInSpectrogram;
}
#if 0
int DetermineIntensePartsOfAudio::calculateNumIntenseParts(const QMultiMap<QRgb, int> &averageIntensitiesForAllFrequenciesAtAllPointsInTimeInSpectrogram, const int spectrogramWidth, const QRgb currentThreshold)
{
    //TODOprobably: have a minimum amount of x pixels (time) that is considered a single "part", instead of just a single pixel. aka group together somehow (average together? idfk) -- some consideration needs to be given into the fact that there might be one continuous/long "intense" part (think:a trumpet being blown for 5 seconds). technically all 5 seconds of that trumpet SHOULD be one 'part'. would we want to report the start timestamp of that intense part, the end, or the start + duration? DETERMINING _DYNAMICALLY_ length'd 'parts' sounds like a bitch (but is surely possible ('edge' detection algorithms (in images, but oh look at that our audio is represented as an image :-D) already exist, it's just a matter of figuring out how to use one)) -- but maybe initially and to KISS, i should just use a simplier (lol typo, keeping it) 'parts' model of some N number of pixels wide

    int numIntenseParts = 0;
    for(int x = 0; x < spectrogramWidth; ++x)
    {
        QRgb averageIntensityForAllFrequenciesAtThatPointInTime = averageIntensitiesForAllFrequenciesAtAllPointsInTimeInSpectrogram.key(x);

        //does that average intensity meet the threshold? if so, increase the number of intense parts
        if(averageIntensityForAllFrequenciesAtThatPointInTime > currentThreshold)
        {
            ++numIntenseParts;
        }
    }
    return numIntenseParts;
}
#endif
QList<qint64> DetermineIntensePartsOfAudio::determineIntensePartsOfAudio(const QMultiMap<QRgb, int> &averageIntensitiesForAllFrequenciesAtAllPointsInTimeInSpectrogram, const QRgb threshold, const int spectrogramPixelsPerSecondOfAudio, const int targetNumIntensePartsInThisSong)
{
    QList<qint64> msTimestampsOfIntensePartsOfAudio;

    QMapIterator<QRgb, int> averageIntensitiesAtGivenPointInTimeIterator(averageIntensitiesForAllFrequenciesAtAllPointsInTimeInSpectrogram);
    averageIntensitiesAtGivenPointInTimeIterator.toBack(); //iterate in reverse because the highest intensities are sorted to the end of the map
    while(averageIntensitiesAtGivenPointInTimeIterator.hasPrevious())
    {
        averageIntensitiesAtGivenPointInTimeIterator.previous();
        if(averageIntensitiesAtGivenPointInTimeIterator.key() <= threshold)
            break; //break instead of continue because we know (since sorted and iterating in reverse) that all the rest will be less than threshold also
        qint64 timestampOfCurrent = determineMsTimestampForXpixelOnSpectrogram(averageIntensitiesAtGivenPointInTimeIterator.value(), spectrogramPixelsPerSecondOfAudio);
        if(!tooCloseToAnotherIntensePart(timestampOfCurrent, msTimestampsOfIntensePartsOfAudio))
        {
            msTimestampsOfIntensePartsOfAudio.append(timestampOfCurrent);
            if(msTimestampsOfIntensePartsOfAudio.size() >= targetNumIntensePartsInThisSong)
                break;
        }
    }
#if 0 //before incorporating m_MinDistanceMsBetweenIntenseParts
    for(int x = 0; x < spectrogramWidth; ++x)
    {
        QRgb averageIntensityForAllFrequenciesAtThatPointInTime = averageIntensitiesForAllFrequenciesAtAllPointsInTimeInSpectrogram.key(x);

        //does that averaged intensity meet the threshold? if so, append it. TODOreq: factor in m_MinDistanceMsBetweenIntenseParts (related TODOmb: should I factor in m_MaxIntensePartsPerSecond in calculateNumIntenseParts as well? (it would implicitly decrease the num intense parts ofc). i am leaning towards YES to this) -- I think the algorithm I want to use to implement this is "find the MOST intense point, then rule out intense points for surrounding it, then repat for the SECOND MOST intense point, and so on until either targetIntensePartsInThisSong is found (likely), or no more intense parts can be found (unlikely/probably-impossible when there's a dynamic threshold -- but could happen if the user manually specified a threshold)
        if(averageIntensityForAllFrequenciesAtThatPointInTime > threshold)
        {
            msTimestampsOfIntensePartsOfAudio.append(determineMsTimestampForXpixelOnSpectrogram(x, spectrogramPixelsPerSecondOfAudio));
        }
    }
#endif

    return msTimestampsOfIntensePartsOfAudio;
}
qint64 DetermineIntensePartsOfAudio::determineMsTimestampForXpixelOnSpectrogram(const int xPixelOnSpectrogramToDetermineTimestampOf, const int spectrogramPixelsPerSecondOfAudio)
{
    //pixel 1 of a spectrogram with 100 pixels per second would have a timestamp of 10ms
    //(1000 * 1) / 100 = 10ms
    return (1000 * xPixelOnSpectrogramToDetermineTimestampOf) / spectrogramPixelsPerSecondOfAudio; //learned cross multiplication in the third grade
}
bool DetermineIntensePartsOfAudio::tooCloseToAnotherIntensePart(qint64 timestampToCheck, const QList<qint64> &msTimestampsOfIntensePartsOfAudioToCheckAgainst)
{
#if 1
    Q_FOREACH(qint64 msTimestampOfIntensePart, msTimestampsOfIntensePartsOfAudioToCheckAgainst)
    {
        if((timestampToCheck >= (msTimestampOfIntensePart-m_MinDistanceMsBetweenIntenseParts)) && (timestampToCheck <= (msTimestampOfIntensePart+m_MinDistanceMsBetweenIntenseParts)))
        {
            return true;
        }
    }
#else
    int half_MinDistanceMsBetweenIntenseParts = m_MinDistanceMsBetweenIntenseParts / 2;
    Q_FOREACH(qint64 msTimestampOfIntensePart, msTimestampsOfIntensePartsOfAudioToCheckAgainst)
    {
        if((timestampToCheck >= (msTimestampOfIntensePart-half_MinDistanceMsBetweenIntenseParts)) && (timestampToCheck <= (msTimestampOfIntensePart+half_MinDistanceMsBetweenIntenseParts)))
        {
            return true;
        }
    }
#endif
    return false;
}
void DetermineIntensePartsOfAudio::startDeterminingIntensePartsOfAudio(const QString &audioFilePath, const int minDistanceMsBetweenIntenseParts)
{
    m_MinDistanceMsBetweenIntenseParts = minDistanceMsBetweenIntenseParts;
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
    soxSpectrogramArgs << audioFilePath << "-n" << "remix" << "1-2" /* TODOoptional: determine num channels via ffprobe, or maybe some other way to mix all channels down to one channel*/ << "rate" << "3k" << "spectrogram" << "-r" << "-h" << "-w" << "Rectangular" << "-X" << QString::number(DetermineIntensePartsOfAudio_spectrogramPixelsPerSecondOfAudio) << "-s" << "-q" << DetermineIntensePartsOfAudio_SOX_Q_ARG << "-o" << m_SoxSpectrogramOutputPngFilePath;
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
    const int maxIntensePartsPerSecond = (1000 / (m_MinDistanceMsBetweenIntenseParts-1)); //the -1 is to account for the fact that integers truncate. so for example 1000 / 3 = 333.333; 1000 / 333 = 2.999something = 2 (instead of 3)
    const int targetNumIntensePartsInThisSong = (maxIntensePartsPerSecond * songDurationSeconds) * 2;
    //const int targetNumIntensePartsInThisSong = (maxIntensePartsPerSecond * songDurationSeconds) / 2; //TODOreq: try uncommenting above 'times two' formula, i'm not sure which I want
    //start with everything qualifying as intense part and then remove until hit target, or vice versa? I suppose it doesn't matter
    QRgb currentThreshold = QColor(255, 255, 255).rgb(); //start with (most probably) nothing qualifying as an intense part, then decrease the threshold until target num intense parts is met. TODOreq: for the default mode of operation, yes it is a good idea to dynamically calculate the threshold, but the user should be able to specify the threshold as an arg as well (overriding the dynamic calculation). the user specifying it would probably (after some trial and error) give much better results
    QMultiMap<QRgb, int> averageIntensitiesForAllFrequenciesAtAllPointsInTimeInSpectrogram = calculateAverageIntensitiesForAllFrequenciesAtAllPointsInTimeInSpectrogram(soxSpectrogramImage);
    //Determine the most intense parts of the audio over and over, lowing the threshold until we meet or exceed targetNumIntensePartsInThisSong
    QList<qint64> msTimestampsOfIntensePartsOfAudio;
    //while(calculateNumIntenseParts(averageIntensitiesForAllFrequenciesAtAllPointsInTimeInSpectrogram, soxSpectrogramWidth, currentThreshold) < targetNumIntensePartsInThisSong)
    do
    {
        msTimestampsOfIntensePartsOfAudio = determineIntensePartsOfAudio(averageIntensitiesForAllFrequenciesAtAllPointsInTimeInSpectrogram, currentThreshold, DetermineIntensePartsOfAudio_spectrogramPixelsPerSecondOfAudio, targetNumIntensePartsInThisSong);
        currentThreshold -= 1000; //TODOoptimization: maybe -= 5 (etc) to speed things up?

    }while(msTimestampsOfIntensePartsOfAudio.size() < targetNumIntensePartsInThisSong);
    //at this point, currentThreshold points to a threshold that makes us slightly exceed (because hitting it exactly would be unlikely) targetIntensePartsInThisSong
    std::sort(msTimestampsOfIntensePartsOfAudio.begin(), msTimestampsOfIntensePartsOfAudio.end()); //we want the emitted list to be in chronological order
    emit doneDeterminingIntensePartsOfAudio(true, msTimestampsOfIntensePartsOfAudio);
}
