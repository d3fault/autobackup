#include "musicfingers.h"

//#include "math.h"
#include <QDebug>

MusicFingers::MusicFingers(QObject *parent)
    : QObject(parent)
{ }
void MusicFingers::moveFinger(Finger::FingerEnum fingerToMove, int newPosition)
{
    qDebug() << fingerToMove << newPosition;
}
