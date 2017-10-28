#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <QObject>

#include <QScopedPointer>

#include "stk/FileLoop.h"

class QAudioOutput;

class StkFileLoopIoDevice;

class AudioOutput : public QObject
{
    Q_OBJECT
public:
    explicit AudioOutput(QObject *parent = nullptr);
private:
    QAudioOutput *m_AudioOutput;
    QScopedPointer<stk::FileLoop> input;
    unsigned int m_NumBufferFrames;
    StkFileLoopIoDevice *m_FileLoopIoDevice;
public slots:
    void startAudioOutput();
    void stopAudioOutput();
    void changePitchShift(stk::StkFloat newPitchShiftValue);
    void changePitchShiftMixAmount(stk::StkFloat newPitchShiftMixAmount);
};

#endif // AUDIOOUTPUT_H
