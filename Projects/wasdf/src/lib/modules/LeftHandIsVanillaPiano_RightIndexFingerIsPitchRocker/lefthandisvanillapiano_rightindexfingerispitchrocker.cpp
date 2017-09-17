#include "lefthandisvanillapiano_rightindexfingerispitchrocker.h"

//class inspired by a memory of my brother playing "ATB - Don't Stop" on the Bechers' keyboard (that had a pitch rocker) -- many many years ago (15+). That was when I knew. that he was (or would become) a musical god. I wasn't wrong :)
//TODOmb: if the "key" is pressed and held down, should the volume fade like with a real piano? it could, but also doesn't have to
LeftHandIsVanillaPiano_RightIndexFingerIsPitchRocker::LeftHandIsVanillaPiano_RightIndexFingerIsPitchRocker(Wasdf *wasdf, QObject *parent)
    : QObject(parent)
    , IWasdfModule(wasdf)
    , IWasdfModuleBasicAudioClipSampler()
{ }
void LeftHandIsVanillaPiano_RightIndexFingerIsPitchRocker::handleFingerMoved(Finger fingerThatMoved, int newPos)
{
    if(Wasdf::fingerIsLeftHand(fingerThatMoved))
    {
        //TODOreq: vanilla piano
        bool isConsideredPressed = wasdf->intRangeToBooleanConverter()->isConsideredPressed(fingerThatMoved, newPos); //TODOoptimization: save the ptr to intRangeToBooleanConverter

        //TODOreq:
        /*
        if(!isConsideredPressed)
        {
            if(isCurrentlyPlayingNote(fingerThatMoved))
            {
                //TODOreq: finger was released, stop playing note
            }
            //else: finger not pressed, not currently playing, do nothing
        }
        else
        {

        }
        */

        if(isConsideredPressed)
        {
            if(!isCurrentlyPlayingNote(fingerThatMoved))
            {
                //TODOreq: play note
                playNote(finger);
            }
        }
    }
    else if(fingerThatMoved == Finger::RightIndex_Finger6)
    {
        //TODOreq: pitch rocker
        //eh since I'm inheritting IWasdfModuleBasicAudioClipSampler for the audio playing functionality, I might have access control problems here. I need to change the pitch of the sample(s) being playing, but they are part of a base class so I don't have [direct] access? idk it might be fine but I might need to refactor, idk atm
    }
    //else (right hand but not index finger): do nothing
}
