#include "libavaudioplayer2.h"

libAvAudioPlayer2::libAvAudioPlayer2(QObject *parent) :
    QObject(parent)
{
}
void libAvAudioPlayer2::init()
{
    m_SharedDecodedAudioBuffer = new ThreadSafeQueueByMutex();

    //loader (curl downloader)
    m_LoaderThread = new QThread();
    m_Loader = new CurlDownloader();
    m_Loader->moveToThread(m_LoaderThread);
    m_LoaderThread->start();

    //decoder
    m_DecoderThread = new QThread();
    m_Decoder = new libAvAudioDecoder(m_SharedDecodedAudioBuffer);
    m_Decoder->moveToThread(m_DecoderThread);
    m_DecoderThread->start();

    //synchronizer
    m_SynchronizerThread = new QThread();
    m_Synchronizer = new Synchronizer();
    m_Synchronizer->moveToThread(m_SynchronizerThread);
    m_SynchronizerThread->start();

    //audio player
    m_AudioThread = new QThread();
    m_AudioPlayer = new QtAudioPlayer(m_SharedDecodedAudioBuffer);
    m_AudioPlayer->moveToThread(m_AudioThread);
    m_AudioThread->start();

    //main connections
    //curl downloader -> audio decoder
    connect(m_Loader, SIGNAL(onDataGathered(QByteArray)), m_Decoder, SLOT(handleNewDataAvailable(QByteArray)));
    //decoder -> syncrhonizer
    //connect(m_Decoder, SIGNAL(onAudioDataDecoded(QByteArray)), m_Synchronizer, SLOT(handleNewAudioDataAvailable(QByteArray)));
    //decoder -> audio player (audio format spec)
    connect(m_Decoder, SIGNAL(onSpecGathered(int sampleRate, int numChannels, int sampleSize)), m_AudioPlayer, SLOT(setAudioSpec(int sampleRate, int numChannels, int sampleSize)));
    //audio player -> synchronizer (notify current usecs)
    connect(m_AudioPlayer, SIGNAL(currentPlayingPositionInUsecs(quint64)), m_Synchronizer, SLOT(audioTimeUpdated(quint64)));
    //synchronizer -> audio player (push)
    //connect(m_Synchronizer, SIGNAL(playAudio(QByteArray)), m_AudioPlayer, SLOT(handleNewAudioBytes(QByteArray)));

    //debug connections
    connect(m_Loader, SIGNAL(d(const QString &)), this, SIGNAL(d(const QString &)));
    connect(m_Decoder, SIGNAL(d(const QString &)), this, SIGNAL(d(const QString &)));
    connect(m_Synchronizer, SIGNAL(d(const QString &)), this, SIGNAL(d(const QString &)));
    connect(m_AudioPlayer, SIGNAL(d(const QString &)), this, SIGNAL(d(const QString &)));

    QMetaObject::invokeMethod(m_Loader, "init", Qt::QueuedConnection);
    //QMetaObject::invokeMethod(m_Decoder, "init", Qt::QueuedConnection); //this is called automatically once we have enough data buffered
}
void libAvAudioPlayer2::play()
{
    //TODO: many potential race conditions if some of these get going before the others =o
    QMetaObject::invokeMethod(m_Synchronizer, "play", Qt::QueuedConnection);
    //QMetaObject::invokeMethod(m_Decoder, "play", Qt::QueuedConnection);
    QMetaObject::invokeMethod(m_Loader, "load", Qt::QueuedConnection);
    QMetaObject::invokeMethod(m_AudioPlayer, "play", Qt::QueuedConnection);
}
