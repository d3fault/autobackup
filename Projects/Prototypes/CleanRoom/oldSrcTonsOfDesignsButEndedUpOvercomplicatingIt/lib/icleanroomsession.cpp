#include "icleanroomsession.h"

#include "cleanroom.h"
#include "cleanroomdoc.h"

ICleanRoomSession::ICleanRoomSession()
    //: QObject(parent)
{ }
void ICleanRoomSession::getAndSubscribeToFrontPage()
{
    //m_CleanRoom->getAndSubscribeToFrontPageBegin();
    ///*emit */getAndSubscribeToFrontPageRequested(this); //the session always passes itself as an api call arg, because it's how the backend returns results to the front end
    emit cleanRoomSessionRequestRequested(new FrontPageCleanRoomSessionRequest(this));
}
