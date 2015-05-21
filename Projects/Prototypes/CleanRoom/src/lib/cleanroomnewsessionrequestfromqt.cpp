#include "cleanroomnewsessionrequestfromqt.h"

#include "cleanroomsession.h"

void CleanRoomNewSessionRequestFromQt::regainContextPossiblyInOtherThread_aka_respondActual(QVariantList responseArgs)
{
    CleanRoomSession *session = responseArgs.first().value<CleanRoomSession*>();
    QMetaObject::invokeMethod(m_ObjectToRespondTo, m_SlotCallback.c_str(), Q_ARG(CleanRoomSession*, session));
}
