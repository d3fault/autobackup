#include "imessagedispenser.h"

IMessageDispenser::IMessageDispenser(QObject *mandatoryOwnerBeforeOwnerHasBeenMoveToThreaded)
    : QObject(mandatoryOwnerBeforeOwnerHasBeenMoveToThreaded) /*<- this is what I'm relying on to get this dispenser to follow business objects when they are .moveToThread'd. It makes the dispenser be a child of the "parent" mandatoryOwner (which is why it's mandatory)*/, m_DestinationObject(0) //TODOreq: before each connection to the destination object, make sure it isn't zero
{

}
void IMessageDispenser::setDestinationObject(IRpcBusinessController *destinationObject)
{
    m_DestinationObject = 0;
}
