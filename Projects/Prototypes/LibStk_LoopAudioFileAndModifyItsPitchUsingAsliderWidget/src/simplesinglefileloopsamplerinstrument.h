#ifndef SIMPLESINGLEFILELOOPSAMPLERINSTRUMENT_H
#define SIMPLESINGLEFILELOOPSAMPLERINSTRUMENT_H

#include "stk/Instrmnt.h"

#include "stk/FileLoop.h"

class SimpleSingleFileLoopSamplerInstrument : public stk::Instrmnt
{
public:
    SimpleSingleFileLoopSamplerInstrument();

    void openFile(const std::string &audioFilePath);

    virtual void noteOn(stk::StkFloat frequency, stk::StkFloat amplitude) override;
    virtual void noteOff(stk::StkFloat amplitude) override;

    virtual void setFrequency(stk::StkFloat frequency) override;

    virtual stk::StkFloat tick(unsigned int channel = 0) override;
    virtual stk::StkFrames& tick(stk::StkFrames &frames, unsigned int channel = 0) override;

    virtual ~SimpleSingleFileLoopSamplerInstrument() = default;

    stk::FileLoop input;
    stk::StkFloat inputGain;
};

#endif // SIMPLESINGLEFILELOOPSAMPLERINSTRUMENT_H
