#include "stevenhereandnow.h"

#include "inheritedtraits/file.h" //The files may be MINE, but the file was given to me by my ancestors (thx btw you mostly dead fucks)

//What happens when you try to code yourself?
//Even though "files" are mostly logic, logic too follows/is-driven-by/is the laws of physics
StevenHereAndNow::StevenHereAndNow(QObject *parent)
    : QObject(parent) //because signals slots and awesome framework
    , IEvolvedMonkey() //because the general dna is mostly the same (not to imply there isn't a huge difference between every single person). we are a clone function after all
{
    m_Files.append(new File("butts.txt"));
}
ILawsOfPhysics *StevenHereAndNow::implementation()
{
    return this; //be sure to override the base implementation, otherwise you're just an evolved monkey (this joke sucks (and is wrong))
}
void StevenHereAndNow::addFile(ILawsOfPhysics *inputFile)
{
    m_Files.append(inputFile);
    emit fileAdded(inputFile);
}
void StevenHereAndNow::handleSayRequested(const QString &whatWasRequestedToBeSaid)
{
    //derp 'shoujld i really say this?'

    emit o(whatWasRequestedToBeSaid); //for now
}
