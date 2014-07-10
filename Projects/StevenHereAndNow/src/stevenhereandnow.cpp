#include "stevenhereandnow.h"

//What happens when you try to code yourself?
StevenHereAndNow::StevenHereAndNow(QObject *parent)
    : QObject(parent)
    , IEvolvedMonkey()
{ }
ILawsOfPhysics *StevenHereAndNow::implementation()
{
    return this; //be sure to override the base implementation, otherwise you're just an evolved monkey (this joke sucks (and is wrong))
}
void StevenHereAndNow::handleSayRequested(const QString &whatWasRequestedToBeSaid)
{
    //derp 'shoujld i really say this?'

    emit o(whatWasRequestedToBeSaid); //for now
}
