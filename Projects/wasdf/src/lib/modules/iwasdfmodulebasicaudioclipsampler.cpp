#include "iwasdfmodulebasicaudioclipsampler.h"

IWasdfModuleBasicAudioClipSampler::IWasdfModuleBasicAudioClipSampler()
{
    FingerIteratorOrderedByMostUsedFingers it;
    while(it.hasNext())
    {
        Finger currentFinger = it.next();
        m_PlayingStatesByFinger.insert(currentFinger, false);
    }
}
bool IWasdfModuleBasicAudioClipSampler::isCurrentlyPlayingNote(Finger finger) const
{
    m_PlayingStatesByFinger::const_iterator it = m_PlayingStatesByFinger.constFind(finger);
    if(it == m_PlayingStatesByFinger.constEnd())
        return false;
    return it->value;
}
void IWasdfModuleBasicAudioClipSampler::playNote(Finger finger)
{
    //TODOreq: libstk integration proper would be nice. synthesizing motherfucking raw waveform would be pro as fuck. stk-demo process2process comm hack solution would probably suffice. I could easily play the vanilla piano .wav files using Qt, but I'm not sure Qt can do the pitch rocker part of it!!! stk otoh I'm pretty sure can, though I don't know the specific SKINI command (or api call if going the lib route) off the top of my head
    //I'm genuinely unsure (forgot this train of thought while I wrote other code)
}
