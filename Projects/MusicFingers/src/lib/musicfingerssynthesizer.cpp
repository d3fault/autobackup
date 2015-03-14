#include "musicfingerssynthesizer.h"

#include <qmath.h>
#include <qendian.h>

#include "musicfingers.h"

MusicFingersSynthesizer::MusicFingersSynthesizer(QObject *parent)
    : QIODevice(parent)
    , m_SampleIndex(0) //TODOmb: smoothly wrap back to zero instead of just overflowing?
{
    m_Fingers.insert(Finger::LeftPinky_A0, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::LeftRing_A1, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::LeftMiddle_A2, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::LeftIndex_A3, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::LeftThumb_A4, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::RightThumb_A5, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::RightIndex_A6, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::RightMiddle_A7, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::RightRing_A8, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::RightPinky_A9, FINGERS_INITIAL_POSITION);
}
void MusicFingersSynthesizer::moveFinger(Finger::FingerEnum fingerToMove, int newPosition)
{
    m_Fingers.insert(fingerToMove, newPosition);
}
void MusicFingersSynthesizer::setLengthOfDataToSynthesizeWhenReadDataIsCalled(qint64 newLengthOfDataToSynthesizeWhenReadDataIsCalled)
{
    m_LengthOfDataToSynthesizeWhenReadDataIsCalled = newLengthOfDataToSynthesizeWhenReadDataIsCalled;
}
qint64 MusicFingersSynthesizer::readData(char *data, qint64 maxlen)
{
    //TODOoptional: make more dynamic, utilizing what QAudioDeviceInfo::nearestFormat gave us
    qint64 totalWritten = 0;
    qint64 toWrite = qMin(m_LengthOfDataToSynthesizeWhenReadDataIsCalled, maxlen);
    while(totalWritten < toWrite)
    {
        //map 0-1023 -> minFreq-maxFreq
        static const qreal minFreq = 50;
        static const qreal maxFreq = 2000;
        qreal a0_fingerPos_AsQreal = static_cast<qreal>(m_Fingers.value(Finger::LeftPinky_A0));
        qreal a0_mappedFrequency = (a0_fingerPos_AsQreal - MUSIC_FINGERS_MIN_POSITION) * (maxFreq - minFreq) / (MUSIC_FINGERS_MAX_POSITION - MUSIC_FINGERS_MIN_POSITION) + minFreq; //jacked from Arduino/WMath.cpp , which in turn was jacked from Wiring. it's just math anyways (not copyrighteable)

        const qreal x = qSin(static_cast<qreal>(2) * static_cast<qreal>(M_PI) * a0_mappedFrequency * qreal(m_SampleIndex % MUSIC_FINGERS_SAMPLE_RATE) / static_cast<qreal>(MUSIC_FINGERS_SAMPLE_RATE));
        qint16 value = static_cast<qint16>(x * 32767);
        qToLittleEndian<qint16>(value, (unsigned char*)(data+totalWritten)); //write directly into the buffer managed by QAudioDevice (data) -- fuck buffering <3
        totalWritten += sizeof(qint16);
        ++m_SampleIndex;
    }
    return totalWritten;
}
qint64 MusicFingersSynthesizer::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data)
    Q_UNUSED(len)
    return 0;
}
qint64 MusicFingersSynthesizer::bytesAvailable() const
{
    return m_LengthOfDataToSynthesizeWhenReadDataIsCalled + bytesAvailable();
}
