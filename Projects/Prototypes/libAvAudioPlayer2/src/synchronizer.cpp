#include "synchronizer.h"

Synchronizer::Synchronizer(QObject *parent) :
    QObject(parent)
{
}
void Synchronizer::audioTimeUpdated(quint64 microsecondsElapsed)
{
    emit d("microseconds played: " + QString::number(microsecondsElapsed));
    //TODO: math looking at pts for video frames etc to determine if we should emit showFrame(QVideoFrame)
    //this class is proving to be quite worthless without video incorporated
}

