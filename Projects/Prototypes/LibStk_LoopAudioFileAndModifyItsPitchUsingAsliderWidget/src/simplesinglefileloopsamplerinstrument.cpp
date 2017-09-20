#include "simplesinglefileloopsamplerinstrument.h"

using namespace stk;

#include <QDebug>

SimpleSingleFileLoopSamplerInstrument::SimpleSingleFileLoopSamplerInstrument()
    : Instrmnt()
    , inputGain(1.0)
{ }
void SimpleSingleFileLoopSamplerInstrument::openFile(const std::string &audioFilePath)
{
    input.openFile(audioFilePath); //throws on error
    qDebug() << "file opened";
}
void SimpleSingleFileLoopSamplerInstrument::noteOn(StkFloat frequency, StkFloat amplitude)
{
    setFrequency(/*frequency*/ input.getFileRate()); //necessary? doesn't the FileLoop already know it's own frequency? wtf? should this be 1.0? 44100? 6? baah
    inputGain = amplitude;
    qDebug() << "note on amplitude:" << amplitude;
    //keyOn();

    qDebug() << "note on";
}
void SimpleSingleFileLoopSamplerInstrument::noteOff(StkFloat amplitude)
{
    //keyOff();

    qDebug() << "note off";
}
void SimpleSingleFileLoopSamplerInstrument::setFrequency(StkFloat frequency)
{
    input.setFrequency(frequency);

    qDebug() << "set frequency:" << frequency;
}
StkFloat SimpleSingleFileLoopSamplerInstrument::tick(unsigned int channel)
{
    //qDebug() << "tick(uint)";

    return (inputGain * input.tick(channel));
}
StkFrames &SimpleSingleFileLoopSamplerInstrument::tick(StkFrames &frames, unsigned int channel)
{
    qDebug() << "tick(StkFrames&)";

    return input.tick(frames/*, channel*/);
}
