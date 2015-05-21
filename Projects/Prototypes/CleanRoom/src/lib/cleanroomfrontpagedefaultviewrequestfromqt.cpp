#include "cleanroomfrontpagedefaultviewrequestfromqt.h"

#include <QVariant>

CleanRoomFrontPageDefaultViewRequestFromQt::CleanRoomFrontPageDefaultViewRequestFromQt(CleanRoom *cleanRoom, QObject *objectCallback, const QString &slotCallback)
    : IFrontPageDefaultViewRequest(cleanRoom)
    , m_ObjectCallback(objectCallback)
    , m_SlotCallback(slotCallback.toStdString())
{ }
#if 0
void CleanRoomFrontPageDefaultViewRequestFromQt::regainContextPossiblyInOtherThread_aka_respondActual(QVariantList responseArgs)
{
    QMetaObject::invokeMethod(m_ObjectCallback, m_SlotCallback.c_str(), Q_ARG(QVariantList, responseArgs));
}
#endif
void CleanRoomFrontPageDefaultViewRequestFromQt::respond(QStringList frontPageDocs)
{
    QMetaObject::invokeMethod(m_ObjectCallback, m_SlotCallback.c_str(), Q_ARG(QStringList, frontPageDocs)); //TODOreq: use signals/slots instead
}
