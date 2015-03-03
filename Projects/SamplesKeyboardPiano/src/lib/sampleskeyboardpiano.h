#ifndef SAMPLESKEYBOARDPIANO_H
#define SAMPLESKEYBOARDPIANO_H

#include <QObject>
#include <QHash>

#include "sampleskeyboardpianoprofile.h"

class QSoundEffect;

class SamplesKeyboardPiano : public QObject
{
    Q_OBJECT
public:
    explicit SamplesKeyboardPiano(QObject *parent = 0);
private:
    //QList<SamplesKeyboardPianoProfile> m_Profiles;
    //QList

    //QHash<Qt::Key, QSoundEffect*> m_KeysToSoundEffectsHash;

    SamplesKeyboardPianoProfile m_CurrentProfile;
    int m_TotalDurationOfAllAudioFilesInCurrentProfilesBaseDir;
    QHash<QString, int> m_AudioFilesInBaseDirAndTheirDurationsInMilliseconds;
    int m_MinSampleDurationMs;
    int m_MaxSampleDurationMs;

    QSoundEffect *soundEffectFrom_FilePath_Offset_And_Duration(const QString &soundEffectFilePath, int soundEffectMillisecondsOffsetIntoFile, int soundEffectMillisecondsDurationOfFile);
    QSoundEffect *soundEffectFromCachedSampleFile(const QString &cachedSampleFilename);
    void ffProbeAudioFilesInBaseDirOfCurrentProfile();
    void ensureCurrentConfigurationOfCurrentProfileHasArandomSampleForEveryKeyboardKey();
    int ffProbeDurationMsFromAudioFile(const QString &audioFilePath);
    QPair<QString /*filePathThatOffsetPointsTo*/,int /*remainingOffsetIntoDeterminedFile*/> determineFileInBaseDirThatOffsetIntoAllFilesPointsTo(int offsetIntoAllFiles);
signals:
    void e(const QString &msg);
    void profilesProbed(QList<SamplesKeyboardPianoProfile> profiles);
public slots:
    void probeProfiles();
    void selectProfile(SamplesKeyboardPianoProfile profile);
    void drum(int keyboardKey, bool shiftKeyPressed);
};

#endif // SAMPLESKEYBOARDPIANO_H
