#include "audiovideosynchronizer.h"

AudioVideoSynchronizer::AudioVideoSynchronizer(QObject *parent) :
    QObject(parent)
{
}
void AudioVideoSynchronizer::start()
{
}
void AudioVideoSynchronizer::queueAudioFrame(const QByteArray &)
{
}
void AudioVideoSynchronizer::queueVideoFrame(const QVideoFrame &)
{
}
