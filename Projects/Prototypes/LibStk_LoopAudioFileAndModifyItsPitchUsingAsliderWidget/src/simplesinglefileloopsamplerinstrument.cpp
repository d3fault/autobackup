#include "simplesinglefileloopsamplerinstrument.h"

using namespace stk;

SimpleSingleFileLoopSamplerInstrument::SimpleSingleFileLoopSamplerInstrument()
    : Instrmnt()
    , inputGain(1.0)
{ }
void SimpleSingleFileLoopSamplerInstrument::openFile(const std::string &audioFilePath)
{
    input.openFile(audioFilePath); //throws on error
}
void SimpleSingleFileLoopSamplerInstrument::noteOn(StkFloat frequency, StkFloat amplitude)
{
    setFrequency(frequency);
    inputGain = amplitude;
    //keyOn();
}
void SimpleSingleFileLoopSamplerInstrument::noteOff(StkFloat amplitude)
{
    //keyOff();
}
void SimpleSingleFileLoopSamplerInstrument::setFrequency(StkFloat frequency)
{
    input.setFrequency(frequency);
}
StkFloat SimpleSingleFileLoopSamplerInstrument::tick(unsigned int channel)
{
    return (inputGain * input.tick(channel));
}
StkFrames &SimpleSingleFileLoopSamplerInstrument::tick(StkFrames &frames, unsigned int channel)
{
    return input.tick(frames/*, channel*/);
}
