#ifndef STKFILELOOPIODEVICE_H
#define STKFILELOOPIODEVICE_H

#include <QIODevice>

#include <QScopedPointer>

#include "stk/FileLoop.h"
#include "stk/PitShift.h"

class StkFileLoopIoDevice : public QIODevice
{
    Q_OBJECT
public:
    StkFileLoopIoDevice(stk::FileLoop *fileLoop, int channels, int numBufferFrames, int bufferSizeBytes, QObject *parent = nullptr);
    void start();
    bool isStarted() const;
    void stop();
protected:
    qint64 bytesAvailable() const;
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
private:
    void refillFramesFromFileLoop();

    bool m_First;
    QScopedPointer<stk::FileLoop> m_FileLoop;
    int m_Channels;
    stk::StkFrames frames;
    size_t m_CurrentIndexIntoFrames;
    stk::PitShift shifter;
    int m_BufferSizeBytes;
public slots:
    void setPitchShift(stk::StkFloat newPitchShift);
    void setPitchShiftMixAmount(stk::StkFloat newPitchShiftMixAmount);
};

#endif // STKFILELOOPIODEVICE_H
