#include "directoriesofaudioandvideofilesmuxersyncercli.h"

#include <QCoreApplication>
#include <QStringList>

#include "directoriesofaudioandvideofilesmuxersyncer.h"

#define ERRRRRR { cliUsage(); emit exitRequested(1); return; }
//#define ERRRRRR { cliUsage(); qApp->exit(1); return; }

#define DirectoriesOfAudioAndVideoFilesMuxerSyncerCli_AUDIO_DELAY_ARG "--audio-delay-ms"
#define DirectoriesOfAudioAndVideoFilesMuxerSyncerCli_TRUNCATE_VIDEOS_TO_MS_ARG "--truncate-videos-to-ms"
#define DirectoriesOfAudioAndVideoFilesMuxerSyncerCli_INTERACTIVELY_CALCULATE_AUDIO_DELAYS_TO_FILE_ARG "--interactively-calculate-audio-delays-to-file"
#define DirectoriesOfAudioAndVideoFilesMuxerSyncerCli_USE_AUDIO_DELAYS_FROM_FILE_ARG "--use-audio-delays-from-file" //TODOreq

DirectoriesOfAudioAndVideoFilesMuxerSyncerCli::DirectoriesOfAudioAndVideoFilesMuxerSyncerCli(QObject *parent)
    : QObject(parent)
    , m_StdIn(stdin)
    , m_StdOut(stdout)
    , m_StdErr(stderr)
{
    connect(this, &DirectoriesOfAudioAndVideoFilesMuxerSyncerCli::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection);

    QStringList arguments = qApp->arguments();
    arguments.removeFirst(); //filename

    qint64 audioDelayMs = 0;
    int audioDelayMsIndex = arguments.indexOf(DirectoriesOfAudioAndVideoFilesMuxerSyncerCli_AUDIO_DELAY_ARG);
    if(audioDelayMsIndex > -1)
    {
        if((arguments.indexOf(DirectoriesOfAudioAndVideoFilesMuxerSyncerCli_INTERACTIVELY_CALCULATE_AUDIO_DELAYS_TO_FILE_ARG) > -1) || (arguments.indexOf(DirectoriesOfAudioAndVideoFilesMuxerSyncerCli_USE_AUDIO_DELAYS_FROM_FILE_ARG) > -1))
        {
            handleE("ERROR: you can't use " DirectoriesOfAudioAndVideoFilesMuxerSyncerCli_AUDIO_DELAY_ARG " with an audio delay file");
            ERRRRRR
        }
        //0 1 2 3 4 (size=5)
        arguments.removeAt(audioDelayMsIndex);
        //0 1 2 4   (size=4)
        if(arguments.size() < (audioDelayMsIndex+1))
            ERRRRRR
        bool convertOk = false;
        audioDelayMs = arguments.takeAt(audioDelayMsIndex).toLongLong(&convertOk);
        if(!convertOk)
            ERRRRRR
    }

    qint64 truncateVideosToMs_OrZeroIfNotToTruncate = 0;
    int truncateVideosToMsIndex = arguments.indexOf(DirectoriesOfAudioAndVideoFilesMuxerSyncerCli_TRUNCATE_VIDEOS_TO_MS_ARG);
    if(truncateVideosToMsIndex > -1)
    {
        arguments.removeAt(truncateVideosToMsIndex);
        if(arguments.size() < (truncateVideosToMsIndex+1))
            ERRRRRR
        bool convertOk = false;
        truncateVideosToMs_OrZeroIfNotToTruncate = arguments.takeAt(truncateVideosToMsIndex).toLongLong(&convertOk);
        if(!convertOk)
            ERRRRRR
    }

    int interactivelyCalculateAudioDelaysToFileIndex = arguments.indexOf(DirectoriesOfAudioAndVideoFilesMuxerSyncerCli_INTERACTIVELY_CALCULATE_AUDIO_DELAYS_TO_FILE_ARG);
    if(interactivelyCalculateAudioDelaysToFileIndex > -1)
    {
        arguments.removeAt(interactivelyCalculateAudioDelaysToFileIndex);
        if(arguments.size() < (interactivelyCalculateAudioDelaysToFileIndex+1))
            ERRRRRR
        m_AudioDelaysOutputFile_OrEmptyStringIfNotInteractivelyCalculatingThem = arguments.takeAt(interactivelyCalculateAudioDelaysToFileIndex);
        if(truncateVideosToMs_OrZeroIfNotToTruncate < 1)
        {
            truncateVideosToMs_OrZeroIfNotToTruncate = 15000;
        }
    }

    QString audioDelaysInputFile_OrEmptyStringIfNoneProvided;
    int useAudioDelaysFromFileIndex = arguments.indexOf(DirectoriesOfAudioAndVideoFilesMuxerSyncerCli_USE_AUDIO_DELAYS_FROM_FILE_ARG);
    if(useAudioDelaysFromFileIndex > -1)
    {
        if(!m_AudioDelaysOutputFile_OrEmptyStringIfNotInteractivelyCalculatingThem.isEmpty())
        {
            handleE("ERROR: you can't use " DirectoriesOfAudioAndVideoFilesMuxerSyncerCli_INTERACTIVELY_CALCULATE_AUDIO_DELAYS_TO_FILE_ARG " and " DirectoriesOfAudioAndVideoFilesMuxerSyncerCli_USE_AUDIO_DELAYS_FROM_FILE_ARG " together"); //TODOoptional: allow it. the import file could just be the 'starting points'
            ERRRRRR
        }
        arguments.removeAt(useAudioDelaysFromFileIndex);
        if(arguments.size() < (useAudioDelaysFromFileIndex+1))
            ERRRRRR
        audioDelaysInputFile_OrEmptyStringIfNoneProvided = arguments.takeAt(useAudioDelaysFromFileIndex);.
    }


#if 1 //fff keeping qcoreapp out of this class means i haven't to exitRequested yet (still in constructor). could solve it a few ways, none of which are 'elegant' :(. Maybe I can/should introduce a "delayedConstructor" that is queued invoked from every "real" constructor, so i can get around the "can't connect to signals emitted in constructor" problem (for EVERY qobject i make, not just these *cli and *gui ones). that tiny bit of non-elegance does buy me a decent amount... (but i am digressing like fuck considering this is all dep of musicfingers xD (i never turn down an opportunity to experiment with improving the elegance of my code (maybe that's a problem (but if i don't do it, i never improve my code))))
    if(arguments.size() < 2)
    {
        ERRRRRR
    }
#endif
    QString directoryOfAudioFiles = arguments.takeFirst();
    QString directoryOfVideoFiles = directoryOfAudioFiles;
    if(arguments.size() > 1)
    {
        directoryOfVideoFiles = arguments.takeFirst();
    }
    QString outputDirectory = arguments.takeFirst();

    DirectoriesOfAudioAndVideoFilesMuxerSyncer *directoriesOfAudioAndVideoFilesMuxerSyncer = new DirectoriesOfAudioAndVideoFilesMuxerSyncer((m_AudioDelaysOutputFile_OrEmptyStringIfNotInteractivelyCalculatingThem.isEmpty() ? DirectoriesOfAudioAndVideoFilesMuxerSyncerMode::NormalMode : DirectoriesOfAudioAndVideoFilesMuxerSyncerMode::InteractivelyCalculateAudioDelaysMode), this);
    connect(directoriesOfAudioAndVideoFilesMuxerSyncer, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(directoriesOfAudioAndVideoFilesMuxerSyncer, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(directoriesOfAudioAndVideoFilesMuxerSyncer, SIGNAL(truncatedPreviewedVideoFileReadyForAudioDelayUserInput(qint64)), this, SLOT(handletruncatedPreviewedVideoFileReadyForAudioDelayUserInput(qint64)));
    connect(this, SIGNAL(userWantsToPreviewThisAudioDelayAgain()), directoriesOfAudioAndVideoFilesMuxerSyncer, SLOT(handleUserWantsToPreviewThisAudioDelayAgain()));
    connect(this, SIGNAL(userWantsToUseCurrentAudioDelayMs()), directoriesOfAudioAndVideoFilesMuxerSyncer, SLOT(handleUserWantsToUseCurrentAudioDelayMs()));
    connect(this, SIGNAL(userWantsToUseCurrentAudioDelayMsForTheRestOfTheVideoFiles()), directoriesOfAudioAndVideoFilesMuxerSyncer, SLOT(handleUserWantsToUseCurrentAudioDelayMsForTheRestOfTheVideoFiles()));
    connect(this, SIGNAL(userInputtedNewAudioDelay(qint64)), directoriesOfAudioAndVideoFilesMuxerSyncer, SLOT(handleUserInputtedNewAudioDelay_SoGenerateAndPreviewIt(qint64)));
    connect(directoriesOfAudioAndVideoFilesMuxerSyncer, SIGNAL(audioDelaysFileCalculatedSoTellMeWhereToSaveIt()), this, SLOT(handleAudioDelaysFileCalculatedSoRespondWithWhereToSaveIt()));
    connect(this, SIGNAL(audioDelaysOutputFilePathChosen(QString)), directoriesOfAudioAndVideoFilesMuxerSyncer, SLOT(handleAudioDelaysOutputFilePathChosen(QString)));
    connect(directoriesOfAudioAndVideoFilesMuxerSyncer, SIGNAL(doneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(bool)), this, SLOT(handleDoneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(bool)));

    QMetaObject::invokeMethod(directoriesOfAudioAndVideoFilesMuxerSyncer, "muxAndSyncDirectoryOfAudioWithDirectoryOfVideo", Q_ARG(QString, directoryOfAudioFiles), Q_ARG(QString, directoryOfVideoFiles), Q_ARG(QString, outputDirectory), Q_ARG(qint64, audioDelayMs), Q_ARG(qint64, truncateVideosToMs_OrZeroIfNotToTruncate), Q_ARG(QString, audioDelaysInputFile_OrEmptyStringIfNoneProvided));
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncerCli::cliUsage()
{
    handleO("Usage: DirectoriesOfAudioAndVideoFilesMuxerSyncerCli srcDir destDir\n\nYou can specify that the audio and video sources are in different directories, in which case you specify the src audio dir first, src video dir second, and of course dest dir last\n\n" DirectoriesOfAudioAndVideoFilesMuxerSyncerCli_AUDIO_DELAY_ARG "\t delay the audio files by this many milliseconds. it can be negative\n\n" DirectoriesOfAudioAndVideoFilesMuxerSyncerCli_TRUNCATE_VIDEOS_TO_MS_ARG "\t truncate all videos to this duration in milliseconds. this is useful for example when trying to calculate the audio delay, saves you from having to process the entire file(s). Note that this option is implied with" DirectoriesOfAudioAndVideoFilesMuxerSyncerCli_INTERACTIVELY_CALCULATE_AUDIO_DELAYS_TO_FILE_ARG ", in which case it defaults to: 15000ms\n\n" DirectoriesOfAudioAndVideoFilesMuxerSyncerCli_INTERACTIVELY_CALCULATE_AUDIO_DELAYS_TO_FILE_ARG "\tRuns in interactive mode for the sole purpose of synchronizing the audio and video files. Only the first 15 seconds of each video is created. That 15 seconds is then shown to you and you are asked to provide an audio delay for it. Each time you enter a new audio delay, a new 15 second preview is shown. You can specify a different preview duration using: " DirectoriesOfAudioAndVideoFilesMuxerSyncerCli_TRUNCATE_VIDEOS_TO_MS_ARG " (note: it's specified in milliseconds). Once you have the audio and video synchronized, you move onto the next video file (or can apply that audio delay to all remaining video files). At the end, the results are spit out into the file you specify. Run this app again specifying that file with " DirectoriesOfAudioAndVideoFilesMuxerSyncerCli_USE_AUDIO_DELAYS_FROM_FILE_ARG " and then the entire video files will be created"); //TODOoptional: don't require two invocations in order to use interactive mode
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncerCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncerCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncerCli::handletruncatedPreviewedVideoFileReadyForAudioDelayUserInput(qint64 currentAudioDelayMs)
{
    QString currentAudioDelayMsString = QString::number(currentAudioDelayMs);
    handleO("Enter a selection:");
    handleO("");
    handleO("U - Use the last used offset (" + currentAudioDelayMsString + ") for this video");
    handleO("A - Use the last used offset (" + currentAudioDelayMsString + ") for this and all of the rest of the videos");
    handleO("P - Re-Preview the video at the same offset");
    handleO("Q - Quit/Abort");
    handleO("");
    handleO("Enter an audio delay in milliseconds to preview (negative numbers are allowed):");
    QString input = m_StdIn.readLine().toLower();
    if(input == "u")
    {
        emit userWantsToUseCurrentAudioDelayMs();
        return;
    }
    if(input == "a")
    {
        emit userWantsToUseCurrentAudioDelayMsForTheRestOfTheVideoFiles(); //this one comes in handy if the video cap device doesn't power down in between capture sessions
        return;
    }
    if(input == "p")
    {
        emit userWantsToPreviewThisAudioDelayAgain();
        return;
    }
    if(input == "q")
    {
        emit exitRequested(1);
        return;
    }
    bool convertOk = false;
    qint64 newAudioDelayMs = input.toLongLong(&convertOk);
    if(convertOk)
    {
        emit userInputtedNewAudioDelay(newAudioDelayMs);
        return;
    }
    handletruncatedPreviewedVideoFileReadyForAudioDelayUserInput(currentAudioDelayMs); //rekt
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncerCli::handleAudioDelaysFileCalculatedSoRespondWithWhereToSaveIt()
{
    emit audioDelaysOutputFilePathChosen(m_AudioDelaysOutputFile_OrEmptyStringIfNotInteractivelyCalculatingThem); //this is a noop in cli mode, where it has to be supplied as a cli arg... but for the GUI we could/would show a save file dialog
}
void DirectoriesOfAudioAndVideoFilesMuxerSyncerCli::handleDoneMuxingAndSyncingDirectoryOfAudioWithDirectoryOfVideo(bool success)
{
    if(!success)
    {
        handleE("muxing and syncing directory of audio with directory of video failed");
    }
    emit exitRequested(success ? 0 : 1);
    //eh still need queued connection because we may not have entered event loop for first time yet... qApp->exit(success ? 0 : 1);
}
