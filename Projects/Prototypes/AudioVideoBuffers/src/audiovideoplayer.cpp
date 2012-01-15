#include "audiovideoplayer.h"

AudioVideoPlayer::AudioVideoPlayer(QObject *parent) :
    QObject(parent)
{
}
void AudioVideoPlayer::initAndProbe()
{
    //init and probe does just that. init's curl and the decoder (until we have enough information to init the audio player)
    //once we have the audio player init'd, then it pulls from the decoder, which pulls from curl. weird, which is why i'm building this prototype to begin with

    m_CurlThread = new QThread();
    m_Curl = new CurlDownloader();
    m_Curl->moveToThread(m_CurlThread);
    m_CurlThread->start();

    m_LocalFileThread = new QThread();
    m_LocalFile = new LocalFile();
    m_LocalFile->moveToThread(m_LocalFileThread);
    m_LocalFileThread->start();


    m_DecoderThread = new QThread();
    m_Decoder = new Decoder();
    m_Decoder->moveToThread(m_DecoderThread);
    m_DecoderThread->start();

    m_AudioPLayerThread = new QThread();
    m_AudioPLayer = new AudioPlayer();
    m_AudioPLayer->moveToThread(m_AudioPLayerThread);
    m_AudioPLayerThread->start();

    connect(m_Curl, SIGNAL(onDataGathered(QByteArray*)), m_LocalFile, SLOT(handleNewData(QByteArray*)));

    QMetaObject::invokeMethod(m_Curl, "start", Qt::QueuedConnection);

}
void AudioVideoPlayer::destroy()
{
}
