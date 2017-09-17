#ifndef IWASDFMODULEBASICAUDIOCLIPSAMPLER_H
#define IWASDFMODULEBASICAUDIOCLIPSAMPLER_H

#include <QHash>

#include "../wasdf.h"
#include "../fingeriteratororderedbymostusedfingers.h"

class IWasdfModuleBasicAudioClipSampler
{
    IWasdfModuleBasicAudioClipSampler();
    virtual ~IWasdfModuleBasicAudioClipSampler() = default;
protected:
    bool isCurrentlyPlayingNote(Finger finger) const;
    void playNote(Finger finger);
private:
    QHash<Finger, bool /*isCurrentlyPlayingAudioClip*/> m_PlayingStatesByFinger;
};

#endif // IWASDFMODULEBASICAUDIOCLIPSAMPLER_H
