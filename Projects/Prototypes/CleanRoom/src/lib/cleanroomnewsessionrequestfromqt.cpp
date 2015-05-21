#include "cleanroomnewsessionrequestfromqt.h"

#include "cleanroomsession.h"

#if 0
void CleanRoomNewSessionRequestFromQt::regainContextPossiblyInOtherThread_aka_respondActual(QVariantList responseArgs)
{
    CleanRoomSession *session = responseArgs.first().value<CleanRoomSession*>();
    QMetaObject::invokeMethod(m_ObjectToRespondTo, m_SlotCallback.c_str(), Q_ARG(CleanRoomSession*, session));
}
#endif
void CleanRoomNewSessionRequestFromQt::respond(CleanRoomSession *session)
{
    QMetaObject::invokeMethod(m_ObjectToRespondTo, m_SlotCallback.c_str(), Q_ARG(CleanRoomSession*, session)); //TODOreq: use signals/slots instead
}
