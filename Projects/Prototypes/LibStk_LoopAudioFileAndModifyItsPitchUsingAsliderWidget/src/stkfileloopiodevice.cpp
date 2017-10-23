#include "stkfileloopiodevice.h"

#include <QDebug>
#include <QThread>

using namespace stk;

StkFileLoopIoDevice::StkFileLoopIoDevice(stk::FileLoop *fileLoop, int channels, QObject *parent)
    : QIODevice(parent)
    , m_First(true)
    , m_FileLoop(fileLoop)
    , m_Channels(channels)
    , m_CurrentIndexIntoFrames(0)
{
    shifter.setEffectMix(0.5); //0.5 gives us mixture of 50% input, 50% effect
}
void StkFileLoopIoDevice::reserveFrames(int numBufferFrames, int channels)
{
    frames.resize(numBufferFrames, channels);
}
void StkFileLoopIoDevice::start()
{
    open(ReadOnly);
}
qint64 StkFileLoopIoDevice::bytesAvailable() const
{
    return m_FileLoop->getSize() + QIODevice::bytesAvailable(); //our file loop NEVER runs out of bytes xD
}
qint64 StkFileLoopIoDevice::readData(char *data, qint64 maxlen)
{
    float *outputBufferAsFloat = (float*)data;
    if(m_First)
    {
        //frames.size would currently return the RESERVED amount, even though it has nothing in it
        m_First = false;
        refillFramesFromFileLoop();

        qDebug() << "StkFileLoopIoDevice::readData thread id:" << QThread::currentThreadId();
    }

    //fill maxlen bytes from frames, refilling frames from m_FileLoop when needed
    qint64 total = 0;
    qint64 stopReadingAtThisManyBytes = maxlen - sizeof(float);
    while(total < stopReadingAtThisManyBytes)
    {
        if(m_CurrentIndexIntoFrames == frames.size())
        {
            refillFramesFromFileLoop();
        }
        *outputBufferAsFloat++ = static_cast<float>(shifter.tick(frames[m_CurrentIndexIntoFrames]));
        total += sizeof(float);
        ++m_CurrentIndexIntoFrames;
    }
    return total;

#if 0
    float *outputBufferAsFloat = (float*)data;
    qint64 total = 0;
    qint64 stopReadingAtThisManyBytes = maxlen - sizeof(float);
    while(total < stopReadingAtThisManyBytes)
    {
        for(int i = 0; i < m_Channels; ++i)
        {
            if(total >= stopReadingAtThisManyBytes)
                return total;
#if 1
            *outputBufferAsFloat++ = static_cast<float>(shifter.tick(m_FileLoop->tick(i))); //read a sample from input buffer, filter it through PitShift effect, then write it to output buffer
#else
            float currentSample = static_cast<float>(shifter.tick(m_FileLoop->tick(i))); //read a sample from input buffer, filter it through PitShift effect
            memcpy(data + total, &currentSample, sizeof(float)); //write it to output buffer
#endif
            total += sizeof(float);
        }
    }
    return total;
#endif
}
qint64 StkFileLoopIoDevice::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data)
    Q_UNUSED(len)
    return 0;
}
void StkFileLoopIoDevice::refillFramesFromFileLoop()
{
    m_FileLoop->tick(frames);
    m_CurrentIndexIntoFrames = 0;
}
void StkFileLoopIoDevice::setPitchShift(StkFloat newPitchShift)
{
    //this won't go into effect WHILE readData is in the middle of executing (because this method won't be called until after it finishes), so that means we need really small buffers to have low as fuck latency. stk uses "512 frames" I think, but idk what QAudioOutput defaults to (it's customizable in any case). using atomics and separate threads might yield even lower latency, but I'm still glad I ported this shit to Qt because I fucking love Qt and c-style 'callbacks' make me fucking vomit
    shifter.setShift(newPitchShift);
}
void StkFileLoopIoDevice::setPitchShiftMixAmount(StkFloat newPitchShiftMixAmount)
{
    shifter.setEffectMix(newPitchShiftMixAmount);
}