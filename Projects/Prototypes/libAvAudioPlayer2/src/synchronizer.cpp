#include "synchronizer.h"

Synchronizer::Synchronizer(QObject *parent) :
    QObject(parent)
{
}
void Synchronizer::init()
{
}
void Synchronizer::handleNeedAudio(int bytesNeeded)
{
    //our audio player calls this (via signal) whenever we need some more audio
    //i'm thinking in a future design with video, THIS is the perfect place to check if we need to emit a video frame to screen
    //maybe not, maybe there should be timers for video... i'm not sure

    //emit playAudio(QByteArray); to return the bytes to our audio player
}
void Synchronizer::handleNewAudioDataAvailable(QByteArray newAudio)
{
}
