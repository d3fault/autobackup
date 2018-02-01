#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <QObject>

#include <QScopedPointer>
#include <QtMultimedia/QAudioFormat>

#include "stk/FileLoop.h"

class QAudioOutput;

class StkFileLoopIoDevice;

class AudioOutput : public QObject
{
    Q_OBJECT
public:
    explicit AudioOutput(QObject *parent = nullptr);
    ~AudioOutput();
private:
    QAudioOutput *m_AudioOutput;
    QScopedPointer<stk::FileLoop> input;
    StkFileLoopIoDevice *m_FileLoopIoDevice;

    void tuneAudioOutputBufferSizeForLowLatency(const QAudioFormat &audioFormat, unsigned int *out_numBufferFrames);
public slots:
    void startAudioOutput();
    void stopAudioOutput();
    void changePitchShift(stk::StkFloat newPitchShiftValue);
    void changePitchShiftMixAmount(stk::StkFloat newPitchShiftMixAmount);
};

#endif // AUDIOOUTPUT_H
