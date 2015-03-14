#include "musicfingerssynthesizer.h"

#include <qmath.h>
#include <qendian.h>

#include "musicfingers.h"

#define MusicFingersSynthesizer_ADD_FINGER(whichFinger) m_Fingers.insert(whichFinger, new Finger(whichFinger, FINGERS_INITIAL_POSITION, this));

MusicFingersSynthesizer::MusicFingersSynthesizer(QObject *parent)
    : QIODevice(parent)
    , m_SampleIndex(0) //TODOmb: smoothly wrap back to zero instead of just overflowing?
{
    MusicFingersSynthesizer_ADD_FINGER(Finger::LeftPinky_A0)
    MusicFingersSynthesizer_ADD_FINGER(Finger::LeftRing_A1)
    MusicFingersSynthesizer_ADD_FINGER(Finger::LeftMiddle_A2)
    MusicFingersSynthesizer_ADD_FINGER(Finger::LeftIndex_A3)
    MusicFingersSynthesizer_ADD_FINGER(Finger::LeftThumb_A4)
    MusicFingersSynthesizer_ADD_FINGER(Finger::RightThumb_A5)
    MusicFingersSynthesizer_ADD_FINGER(Finger::RightIndex_A6)
    MusicFingersSynthesizer_ADD_FINGER(Finger::RightMiddle_A7)
    MusicFingersSynthesizer_ADD_FINGER(Finger::RightRing_A8)
    MusicFingersSynthesizer_ADD_FINGER(Finger::RightPinky_A9)
}
void MusicFingersSynthesizer::moveFinger(Finger::FingerEnum fingerToMove, int newPosition)
{
    m_Fingers.value(fingerToMove)->animatedMoveFingerPosition(newPosition);
}
void MusicFingersSynthesizer::setLengthOfDataToSynthesizeWhenReadDataIsCalled(qint64 newLengthOfDataToSynthesizeWhenReadDataIsCalled)
{
    m_LengthOfDataToSynthesizeWhenReadDataIsCalled = newLengthOfDataToSynthesizeWhenReadDataIsCalled;
}
//TODOreq: periodic (~3 seconds) small popping sound even when the finger position doesn't change
//TODOreq: the changing of finger positions needs to sound like the sox command "play -n synth .5 sin 90-3000" (SMOOTH, NOT CHOPPY), where 90 is the old position, 3000 is the new position, and .5 is the duration.
//^THE PROBLEM: by simply changing the frequency (like I do now), it doesn't account for all of the sine wave that has already been generated ("to the left" on the graph, but really just in the past (or for lesser values of m_SampleIndex))... so when we call sin() on the new frequency, the value is very unlikely to be near where it was the last time we called sin() with the previous freqency
//^THE (A) SOLUTION: translate, on the x-axis, the sin wave of the new frequency so that it matches the value that the old frequency would have generated at that value of x (a hypothetical calculation, since we did send sin(x-1) for the old frequency to speakers, but we don't want to send sin(x) for the old frequency to the speakers, we just want it's value for matching it up with sin(x) for the new frequency), AND ADDITIIONALLY ensure that the sin graph is "going the same direction" (increasing vs decreasing)... which basically means just translating on the x axis until the next intersection. ---- HOWEVER I'm not sure this solution is decent, maybe I need to do what I describe but while additionally doing an "animation" from the old frequency to the new. without such animation, it might still pop even if the y value does pick up right where the old frequency left it off -- I think that QPropertyAnimation kind of does what I'm describing, HOWEVER maybe I need to be animating from _within_ the main synthesis loop (the "while(totalWritten < toWrite)" loop below)... whereas my QPropertyAnimation can only animate in between calls to readData. Maybe it is possible for me to still use QAbstractAnimation from within the main synth loop, perhaps driving it manually? Calling QVariantAnimation::interpolated myself from the main synth loop might do the trick (which I guess means I never start the animation?)... I mean I know it's not necessary, but might as well outsource the math <3. I'm still unsure as fuck how to "translate" the sin wave on the x-axis like described earlier in this paragraph :(... I only know it's possible...
//"so much to code, so little time" vs. "nothing to do, so reinventing the wheel for fun". I guess it depends on the task at hand. I so totally DON'T want to use an existing synth library here
qint64 MusicFingersSynthesizer::readData(char *data, qint64 maxlen)
{
    //TODOoptional: make more dynamic, utilizing what QAudioDeviceInfo::nearestFormat gave us
    qint64 totalWritten = 0;
    qint64 toWrite = qMin(m_LengthOfDataToSynthesizeWhenReadDataIsCalled, maxlen);

    //map 0-1023 -> minFreq-maxFreq
    static const qreal minFreq = 50;
    static const qreal maxFreq = 2000;
    qreal a0_fingerPos_AsQreal = static_cast<qreal>(m_Fingers.value(Finger::LeftPinky_A0)->position());
    qreal a0_mappedFrequency = (a0_fingerPos_AsQreal - MUSIC_FINGERS_MIN_POSITION) * (maxFreq - minFreq) / (MUSIC_FINGERS_MAX_POSITION - MUSIC_FINGERS_MIN_POSITION) + minFreq; //jacked from Arduino/WMath.cpp , which in turn was jacked from Wiring. it's just math anyways (not copyrightable)

    //Q_ASSERT(toWrite % sizeof(qint16) == 0)

    static const qreal twoPi = static_cast<qreal>(2) * static_cast<qreal>(M_PI); //OT: i don't think static const's in methods are thread-safe xD, could be wrong
    const qreal twoPiTimesMappedFrequency = twoPi * a0_mappedFrequency;

    while(totalWritten < toWrite)
    {
        //const qreal y = qSin(2 * M_PI * a0_mappedFrequency * qreal(m_SampleIndex % MUSIC_FINGERS_SAMPLE_RATE) / MUSIC_FINGERS_SAMPLE_RATE);
        const qreal y = qSin(twoPiTimesMappedFrequency * qreal(m_SampleIndex % MUSIC_FINGERS_SAMPLE_RATE) / static_cast<qreal>(MUSIC_FINGERS_SAMPLE_RATE));
        qint16 yAsShort = static_cast<qint16>(y * 32767);
        qToLittleEndian<qint16>(yAsShort, (unsigned char*)(data+totalWritten)); //write directly into the buffer managed by QAudioDevice (data) -- fuck buffering <3
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
