#ifndef MUSICFINGERSSYNTHESIZER_H
#define MUSICFINGERSSYNTHESIZER_H

#include <QIODevice>

#include <QHash>

#include "finger.h"

class MusicFingersSynthesizer : public QIODevice
{
    Q_OBJECT
public:
    MusicFingersSynthesizer(QObject *parent);
private:
    friend class MusicFingers;

    QHash<Finger::FingerEnum /*fingerEnum*/, Finger* /*finger*/> m_Fingers;
    //QHash<Finger::FingerEnum /*finger*/, QPropertyAnimation* /*transitionToPosition*/> m_FingerTransitionAnimations;
    long long m_SampleIndex;
    qint64 m_LengthOfDataToSynthesizeWhenReadDataIsCalled;

    void moveFinger(Finger::FingerEnum fingerToMove, int newPosition);
    void setLengthOfDataToSynthesizeWhenReadDataIsCalled(qint64 newLengthOfDataToSynthesizeWhenReadDataIsCalled);
protected:
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
    qint64 bytesAvailable() const;
};

#endif // MUSICFINGERSSYNTHESIZER_H
