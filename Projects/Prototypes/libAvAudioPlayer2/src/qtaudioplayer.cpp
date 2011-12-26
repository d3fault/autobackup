#include "qtaudioplayer.h"

QtAudioPlayer::QtAudioPlayer(QObject *parent) :
    QObject(parent)
{
}
void QtAudioPlayer::init()
{
}
void QtAudioPlayer::setAudioSpec(int sampleRate, int numChannels, int sampleSize)
{
}
void QtAudioPlayer::handleNewAudioBytes(QByteArray newAudioBytes)
{
}
