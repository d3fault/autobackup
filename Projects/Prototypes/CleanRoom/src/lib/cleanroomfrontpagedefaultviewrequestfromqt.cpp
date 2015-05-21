#include "cleanroomfrontpagedefaultviewrequestfromqt.h"

#include <QVariant>

CleanRoomFrontPageDefaultViewRequestFromQt::CleanRoomFrontPageDefaultViewRequestFromQt(CleanRoomSession *session, QObject *objectCallback, const char *slotCallback)
    : ICleanRoomFrontPageDefaultViewRequest(session)
    , m_ObjectCallback(objectCallback)
    , m_SlotCallback(slotCallback)
{ }
void CleanRoomFrontPageDefaultViewRequestFromQt::respondActual(QVariantList responseArgs)
{
    QMetaObject::invokeMethod(m_ObjectCallback, m_SlotCallback, Q_ARG(QVariantList, responseArgs));
}
