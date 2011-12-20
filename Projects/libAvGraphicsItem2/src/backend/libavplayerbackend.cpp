#include "libavplayerbackend.h"

#include "libavdecoder.h"
#include "audiovideosynchronizer.h"

LibAvPlayerBackend::LibAvPlayerBackend(QObject *parent) :
    QObject(parent)
{
}
void LibAvPlayerBackend::init()
{
    privConstructor();

    QMetaObject::invokeMethod(m_Synchronizer, "init", Qt::QueuedConnection);
    QMetaObject::invokeMethod(m_Decoder, "init", Qt::QueuedConnection);
}
void LibAvPlayerBackend::privConstructor()
{
    m_DecoderThread = new QThread();
    m_Decoder = new LibAvDecoder();
    m_Decoder->moveToThread(m_DecoderThread);
    m_DecoderThread->start();

    m_SynchronizerThread = new QThread();
    m_Synchronizer = new AudioVideoSynchronizer();
    m_Synchronizer->moveToThread(m_SynchronizerThread);
    m_SynchronizerThread->start();

    connect(m_Decoder, SIGNAL(audioFrameDecoded(const QByteArray &)), m_Synchronizer, SLOT(queueAudioFrame(const QByteArray &)));
    connect(m_Decoder, SIGNAL(videoFrameDecoded(const QVideoFrame &)), m_Synchronizer, SLOT(queueVideoFrame(const QVideoFrame &)));

    connect(m_Synchronizer, SIGNAL(audioReadyToBePresented(const QByteArray &)), this, SIGNAL(audioReadyToBePresented(const QByteArray &)));
    connect(m_Synchronizer, SIGNAL(frameReadyToBePresented(const QVideoFrame &)), this, SIGNAL(frameReadyToBePresented(const QVideoFrame &)));

    connect(this, SIGNAL(onPlay()), m_Decoder, SLOT(play()));
    connect(this, SIGNAL(onPlay()), m_Synchronizer, SLOT(play()));
}
void LibAvPlayerBackend::play()
{
    emit onPlay();
}
