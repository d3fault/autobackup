#include "sampleskeyboardpiano.h"

#include <QCoreApplication>
#include <QSettings>
#include <QKeySequence>
#include <QFileInfo>
#include <QProcess>
#include <QDateTime>
#include <QDirIterator>
#include <QTextStream>
#include <QHashIterator>

#define SamplesKeyboardPiano_SETTINGS_PROFILES_ARRAY_KEY "profiles"
#define SamplesKeyboardPiano_SETTINGS_PROFILE_NAME_KEY "name"
#define SamplesKeyboardPiano_SETTINGS_PROFILE_BASEDIR_KEY "baseDir"
#define SamplesKeyboardPiano_SETTINGS_PROFILE_CURRENTCONFIGURATIONINDEX_KEY "currentConfigurationIndex"
#define SamplesKeyboardPiano_SETTINGS_PROFILE_CONFIGURATIONS_ARRAY_KEY "configurations"
#define SamplesKeyboardPiano_SETTINGS_PROFILE_CONFIGURATION_SOUNDEFFECTFILEPATH_KEY "configurationSoundEffectFilePath"
#define SamplesKeyboardPiano_SETTINGS_PROFILE_CONFIGURATION_SOUNDEFFECTMILLSECONDSOFFSETINTOFILE_KEY "configurationSoundEffectMillisecondsOffsetIntoFile"
#define SamplesKeyboardPiano_SETTINGS_PROFILE_CONFIGURATION_SOUNDEFFECTMILLSECONDSDURATIONOFFILE_KEY "configurationSoundEffectMillisecondsDurationOfFile"

#define SamplesKeyboardPiano_CACHE_DIR_WITH_SLASH_APPENDED "/run/shm/" //TODOoptional: runtime arg, or profile value

SamplesKeyboardPiano::SamplesKeyboardPiano(QObject *parent)
    : QObject(parent)
    , m_TotalDurationOfAllAudioFilesInCurrentProfilesBaseDir(0)
    , m_MinSampleDurationMs(100) //TODOreq: this and max should be runtime changeable via ui, and the changes should maybe be reflected in the timeline (but maybe not, since the 'results of' the change (randomly selected samples + offsets + durations) would already be reflected)
    , m_MaxSampleDurationMs(1000)
{
    QCoreApplication::setApplicationName("SamplesKeyboardPiano");
    QCoreApplication::setOrganizationName("SamplesKeyboardPianoOrganization");
    QCoreApplication::setOrganizationDomain("SamplesKeyboardPianoDomain");

#if 0
    QSoundEffect *soundEffect = new QSoundEffect(this);
    soundEffect->setSource(QUrl::fromLocalFile());
#endif
}
QSoundEffect *SamplesKeyboardPiano::soundEffectFrom_FilePath_Offset_And_Duration(const QString &soundEffectFilePath, int soundEffectMillisecondsOffsetIntoFile, int soundEffectMillisecondsDurationOfFile) //TODOoptional: might be good idea to delete the cached sample files at some point
{
    //TODOoptimization: check the output doesn't already exist (if it does, use it)

    QFileInfo cachedFileInfo(SamplesKeyboardPiano_CACHE_DIR_WITH_SLASH_APPENDED + soundEffectFilePath);
    QDir pathMaker(cachedFileInfo.path());
    if(!pathMaker.exists())
    {
        pathMaker.mkpath(pathMaker.path());
    }

    QString cachedSampleFilename = SamplesKeyboardPiano_CACHE_DIR_WITH_SLASH_APPENDED + soundEffectFilePath + "_" + QString::number(soundEffectMillisecondsOffsetIntoFile) + "_" + QString::number(soundEffectMillisecondsDurationOfFile) + ".wav";

    if(QFileInfo::exists(cachedSampleFilename))
    {
        return soundEffectFromCachedSampleFile(cachedSampleFilename);
    }

    QProcess ffmpegProcess(this);
    ffmpegProcess.setProcessChannelMode(QProcess::MergedChannels);
    QStringList ffmpegArgs;
    double millisecondsOffsetAsDouble = static_cast<double>(soundEffectMillisecondsOffsetIntoFile);
    double millisecondsOffsetAsDecimal = millisecondsOffsetAsDouble * 0.001;
    double millsecondsDurationAsDouble = static_cast<double>(soundEffectMillisecondsDurationOfFile);
    double millisecondsDurationAsDecimal =  millsecondsDurationAsDouble * 0.001;
    ffmpegArgs << "-ss" << QString::number(millisecondsOffsetAsDecimal, 'f') << "-i" << soundEffectFilePath << "-t" << QString::number(millisecondsDurationAsDecimal, 'f') << cachedSampleFilename;
    ffmpegProcess.start("ffmpeg", ffmpegArgs, QIODevice::ReadOnly);
    if(!ffmpegProcess.waitForStarted())
    {
        emit e(ffmpegProcess.errorString());
        emit e("ffmpeg failed to start");
        return 0;
    }
    if(!ffmpegProcess.waitForFinished(-1))
    {
        emit e(ffmpegProcess.errorString());
        emit e("ffmpeg failed to finish");
        return 0;
    }
    if((ffmpegProcess.exitCode() != 0) || (ffmpegProcess.exitStatus() != QProcess::NormalExit))
    {
        emit e(ffmpegProcess.errorString());
        emit e(ffmpegProcess.readAll());
        emit e("ffmpeg exitted abnormally");
        return 0;
    }

    return soundEffectFromCachedSampleFile(cachedSampleFilename);
}
QSoundEffect *SamplesKeyboardPiano::soundEffectFromCachedSampleFile(const QString &cachedSampleFilename)
{
    QSoundEffect *soundEffect = new QSoundEffect(this); //TODOreq: use better memory utilization (right now every configuration of every profile is loaded at startup xD)
    soundEffect->setSource(QUrl::fromLocalFile(cachedSampleFilename));
    return soundEffect;
}
void SamplesKeyboardPiano::ffProbeAudioFilesInBaseDirOfCurrentProfile()
{
    m_TotalDurationOfAllAudioFilesInCurrentProfilesBaseDir = 0;
    m_AudioFilesInBaseDirAndTheirDurationsInMilliseconds.clear();
    QStringList audioFiles;
    audioFiles << "*.wav" << "*.mp3" << "*.wma" << "*.opus" << "*.flac";
    QDirIterator baseDirIterator(m_CurrentProfile.BaseDir.path(), audioFiles, (QDir::AllEntries | QDir::NoDotAndDotDot), QDirIterator::Subdirectories);
    while(baseDirIterator.hasNext())
    {
        baseDirIterator.next();
        QFileInfo currentFileInfo = baseDirIterator.fileInfo();
        if(!currentFileInfo.isFile())
            continue;
        const QString &currentFilePath = currentFileInfo.filePath();
        int durationOfCurrentAudioFileInMillseconds = ffProbeDurationMsFromAudioFile(currentFileInfo.filePath());
        if(durationOfCurrentAudioFileInMillseconds > 0)
        {
            m_TotalDurationOfAllAudioFilesInCurrentProfilesBaseDir += durationOfCurrentAudioFileInMillseconds;
            m_AudioFilesInBaseDirAndTheirDurationsInMilliseconds.insert(currentFilePath, durationOfCurrentAudioFileInMillseconds);
        }
        else
        {
            emit e(currentFilePath + " had duration of: " + QString::number(durationOfCurrentAudioFileInMillseconds));
        }
    }
}
void SamplesKeyboardPiano::ensureCurrentConfigurationOfCurrentProfileHasArandomSampleForEveryKeyboardKey()
{
    for(int i = Qt::Key_A; i <= Qt::Key_Z; ++i)
    {
        QSoundEffect *sampleAttachedToCurrentKeyboardKey = m_CurrentProfile.CurrentConfiguration.KeyboardKeysToSamplesHash.value(i, 0);
        if(sampleAttachedToCurrentKeyboardKey == 0)
        {
            //get new random sample for this keyboard key
            qsrand(QDateTime::currentMSecsSinceEpoch());
            int offsetIntoAllFiles = (qrand() % m_TotalDurationOfAllAudioFilesInCurrentProfilesBaseDir); //TODOmb: "long songs" have higher likelyhood of a sample being selected from them. might be better off doing a rand() with the numSongs first, then another rand() for the offset therein. TODOoptional: qrand() mod'ing is not uniformly distributed
            int duration = (qrand() % (m_MaxSampleDurationMs - m_MinSampleDurationMs)); //TODOprobably: we need to account for "end of songs"... there might not be enough duration left! we COULD grab the ms from the beginning 'next' song, but chances are extremely high it won't sound good with the ending of the 'current' song...
            QPair<QString /*filePathThatOffsetPointsTo*/,int /*remainingOffsetIntoDeterminedFile*/> filePathAndRemainingOffset = determineFileInBaseDirThatOffsetIntoAllFilesPointsTo(offsetIntoAllFiles);
            sampleAttachedToCurrentKeyboardKey = soundEffectFrom_FilePath_Offset_And_Duration(filePathAndRemainingOffset.first, filePathAndRemainingOffset.second, duration);
            if(sampleAttachedToCurrentKeyboardKey)
            {
                m_CurrentProfile.CurrentConfiguration.KeyboardKeysToSamplesHash.insert(i, sampleAttachedToCurrentKeyboardKey); //TODOreq: QSettings::save() -- and also timeline'ize xD
            }
        }
    }
}
int SamplesKeyboardPiano::ffProbeDurationMsFromAudioFile(const QString &audioFilePath)
{
    QProcess ffProbeProcess(this);
    QStringList ffProbeArgs;
    ffProbeArgs << "-show_format" << audioFilePath;
    ffProbeProcess.start("ffprobe", ffProbeArgs, QIODevice::ReadOnly);
    if(!ffProbeProcess.waitForStarted())
    {
        emit e(ffProbeProcess.errorString());
        emit e("ffprobe failed to start");
        return -1;
    }
    if(!ffProbeProcess.waitForFinished(-1))
    {
        emit e(ffProbeProcess.errorString());
        emit e("ffprobe failed to finish");
        return -1;
    }
    if(ffProbeProcess.exitCode() != 0 || ffProbeProcess.exitStatus() != QProcess::NormalExit)
    {
        emit e(ffProbeProcess.errorString());
        emit e(ffProbeProcess.readAll());
        emit e("ffprobe exitted abnormally");
        return -1;
    }

    QTextStream ffProbeTextStream(&ffProbeProcess);
    QString currentFfProbeLine;
    bool inFormatSection = false;
    while(!ffProbeTextStream.atEnd())
    {
        currentFfProbeLine = ffProbeTextStream.readLine().trimmed();
        if(inFormatSection)
        {
            if(currentFfProbeLine == "[/FORMAT]")
            {
                inFormatSection = false;
            }
            else if(currentFfProbeLine.startsWith("duration="))
            {
                if(currentFfProbeLine.contains("N/A"))
                {
                    return -1;
                }
                QStringList splitAtEqualSign = currentFfProbeLine.split("=", QString::SkipEmptyParts);
                if(splitAtEqualSign.size() != 2)
                {
                    return -1;
                }
                bool ok = false;
                QString rightHalf = splitAtEqualSign.at(1);
                double parsedDurationSeconds = rightHalf.toDouble(&ok);
                if(!ok)
                {
                    return -1;
                }
                return (parsedDurationSeconds*100);
            }
        }
        else if(currentFfProbeLine == "[FORMAT]")
        {
            inFormatSection = true;
        }
    }
    return -1;
}
QPair<QString, int> SamplesKeyboardPiano::determineFileInBaseDirThatOffsetIntoAllFilesPointsTo(int offsetIntoAllFiles)
{
    QPair<QString,int> ret;
    QHashIterator<QString,int> it(m_AudioFilesInBaseDirAndTheirDurationsInMilliseconds);
    while(it.hasNext())
    {
        it.next();
        if((offsetIntoAllFiles - it.value()) < 0)
        {

            ret.first = it.key();
            ret.second = offsetIntoAllFiles;
            return ret;
        }
        offsetIntoAllFiles -= it.value();
    }
    emit e("got to end of determineFileInBaseDirThatOffsetIntoAllFilesPointsTo. this should never happen");
    return ret; //maybe populate with hash.first()? but we don't know it exists xD
}
void SamplesKeyboardPiano::probeProfiles()
{
    QSettings settings;
    QList<SamplesKeyboardPianoProfile> profiles;
    int numProfiles = settings.beginReadArray(SamplesKeyboardPiano_SETTINGS_PROFILES_ARRAY_KEY);
    for(int i = 0; i < numProfiles; ++i)
    {
        settings.setArrayIndex(i);
        SamplesKeyboardPianoProfile profile;
        profile.Name = settings.value(SamplesKeyboardPiano_SETTINGS_PROFILE_NAME_KEY).toString();
        profile.BaseDir.setPath(settings.value(SamplesKeyboardPiano_SETTINGS_PROFILE_BASEDIR_KEY).toString());

        int numConfigurations = settings.beginReadArray(SamplesKeyboardPiano_SETTINGS_PROFILE_CONFIGURATIONS_ARRAY_KEY);
        for(int j = 0; j < numConfigurations; ++j)
        {
            settings.setArrayIndex(j);
            SamplesKeyboardPianoProfileConfiguration configuration;
            for(int currentKeyboardKey = Qt::Key_A; currentKeyboardKey <= Qt::Key_Z; ++currentKeyboardKey)
            {
                settings.beginGroup(QKeySequence(currentKeyboardKey).toString());
                QString soundEffectFilePath = settings.value(SamplesKeyboardPiano_SETTINGS_PROFILE_CONFIGURATION_SOUNDEFFECTFILEPATH_KEY).toString();
                int soundEffectMillisecondsOffsetIntoFile = settings.value(SamplesKeyboardPiano_SETTINGS_PROFILE_CONFIGURATION_SOUNDEFFECTMILLSECONDSOFFSETINTOFILE_KEY).toInt();
                int soundEffectMillisecondsDurationOfFile = settings.value(SamplesKeyboardPiano_SETTINGS_PROFILE_CONFIGURATION_SOUNDEFFECTMILLSECONDSDURATIONOFFILE_KEY).toInt();
                settings.endGroup();
                configuration.KeyboardKeysToSamplesHash.insert(currentKeyboardKey, soundEffectFrom_FilePath_Offset_And_Duration(soundEffectFilePath, soundEffectMillisecondsOffsetIntoFile, soundEffectMillisecondsDurationOfFile));
            }
            profile.Configurations.append(configuration);
        }
        settings.endArray();

        int currentConfigurationIndex = settings.value(SamplesKeyboardPiano_SETTINGS_PROFILE_CURRENTCONFIGURATIONINDEX_KEY).toInt();;
        profile.CurrentConfiguration = profile.Configurations.at(currentConfigurationIndex);

        profiles.append(profile);
    }
    settings.endArray();
    emit profilesProbed(profiles);
}
void SamplesKeyboardPiano::selectProfile(SamplesKeyboardPianoProfile profile)
{
    m_CurrentProfile = profile;
    ffProbeAudioFilesInBaseDirOfCurrentProfile();
    if(profile.Configurations.isEmpty())
    {
        SamplesKeyboardPianoProfileConfiguration newConfiguration;
        profile.Configurations.append(newConfiguration);
        profile.CurrentConfiguration = newConfiguration;
    }
    ensureCurrentConfigurationOfCurrentProfileHasArandomSampleForEveryKeyboardKey();
}
void SamplesKeyboardPiano::drum(int keyboardKey, bool shiftKeyPressed)
{
    if(shiftKeyPressed)
    {
        //TODOreq: randomly select/set new sample. TODOoptional: make note of it in replayeable timeline
        return;
    }

    //QSoundEffect *soundEffect = m_KeysToSoundEffectsHash.value(key);
    QSoundEffect *soundEffect = m_CurrentProfile.CurrentConfiguration.KeyboardKeysToSamplesHash.value(keyboardKey, 0);
    if(soundEffect)
        soundEffect->play();
}
